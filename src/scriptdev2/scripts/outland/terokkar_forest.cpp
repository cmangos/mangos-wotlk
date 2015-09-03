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
SDComment: Quest support: 9889, 10009, 10051, 10052, 10446/10447, 10852, 10873, 10887, 10896, 10898, 10922, 10988, 11085, 11093, 11096.
SDCategory: Terokkar Forest
EndScriptData */

/* ContentData
mob_unkor_the_ruthless
mob_netherweb_victim
npc_akuno
npc_hungry_nether_ray
npc_letoll
npc_mana_bomb_exp_trigger
go_mana_bomb
go_veil_skith_cage
npc_captive_child
npc_isla_starmane
npc_skywing
npc_cenarion_sparrowhawk
npc_skyguard_prisoner
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "pet_ai.h"

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
        m_creature->DeleteThreatList();
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

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
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

CreatureAI* GetAI_mob_unkor_the_ruthless(Creature* pCreature)
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
        SetCombatMovement(false);
        Reset();
    }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* pKiller) override
    {
        if (Player* pPlayer = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (pPlayer->GetQuestStatus(QUEST_TAKEN_IN_NIGHT) == QUEST_STATUS_INCOMPLETE)
            {
                if (!urand(0, 3))
                {
                    m_creature->SummonCreature(NPC_FREED_WARRIOR, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 60000);
                    pPlayer->KilledMonsterCredit(NPC_FREED_WARRIOR, m_creature->GetObjectGuid());
                }
                else
                    m_creature->SummonCreature(netherwebVictims[urand(0, 5)], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 60000);
            }
        }
    }
};

CreatureAI* GetAI_mob_netherweb_victim(Creature* pCreature)
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
            case 5:
                DoScriptText(SAY_AKU_AMBUSH_A, m_creature);
                m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                break;
            case 14:
                DoScriptText(SAY_AKU_AMBUSH_B, m_creature);

                if (Creature* pTemp = m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB1[0], m_afAmbushB1[1], m_afAmbushB1[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000))
                    DoScriptText(SAY_AKU_AMBUSH_B_REPLY, pTemp);

                m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB2[0], m_afAmbushB2[1], m_afAmbushB2[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                break;
            case 15:
                SetRun();
                break;
            case 18:
                DoScriptText(SAY_AKU_COMPLETE, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPING_TOMB, m_creature);

                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
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

CreatureAI* GetAI_npc_akuno(Creature* pCreature)
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

    void Reset() override { }

    void OwnerKilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetEntry() == NPC_BLACK_WARP_CHASER)
        {
            // Distance expected?
            if (m_creature->IsWithinDistInMap(pVictim, 10.0f))
            {
                DoScriptText(EMOTE_FEED, m_creature);
                m_creature->CastSpell(m_creature, SPELL_FEED_CREDIT, true);
            }
        }
    }
};

CreatureAI* GetAI_npc_hungry_nether_ray(Creature* pCreature)
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

    std::list<Creature*> m_lResearchersList;

    uint32 m_uiEventTimer;
    uint32 m_uiEventCount;

    void Reset() override {}

    // will make them follow, but will only work until they enter combat with any unit
    void SetFormation()
    {
        uint32 uiCount = 0;

        for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            float fAngle = uiCount < MAX_RESEARCHER ? M_PI / MAX_RESEARCHER - (uiCount * 2 * M_PI / MAX_RESEARCHER) : 0.0f;

            if ((*itr)->isAlive() && !(*itr)->isInCombat())
                (*itr)->GetMotionMaster()->MoveFollow(m_creature, 2.5f, fAngle);

            ++uiCount;
        }
    }

    Creature* GetAvailableResearcher(uint8 uiListNum)
    {
        if (!m_lResearchersList.empty())
        {
            uint8 uiNum = 1;

            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
            {
                if (uiListNum && uiListNum != uiNum)
                {
                    ++uiNum;
                    continue;
                }

                if ((*itr)->isAlive() && (*itr)->IsWithinDistInMap(m_creature, 20.0f))
                    return (*itr);
            }
        }

        return NULL;
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
            case 0:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_LE_KEEP_SAFE, m_creature, pPlayer);
                break;
            case 1:
                DoScriptText(SAY_LE_NORTH, m_creature);
                break;
            case 10:
                DoScriptText(SAY_LE_ARRIVE, m_creature);
                break;
            case 12:
                DoScriptText(SAY_LE_BURIED, m_creature);
                SetEscortPaused(true);
                break;
            case 13:
                SetRun();
                break;
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->isInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
            DoScriptText(SAY_LE_HELP_HIM, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (pPlayer && pPlayer->isAlive())
            pSummoned->AI()->AttackStart(pPlayer);
        else
            pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
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
                            m_creature->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                            break;
                        case 13:
                            DoScriptText(EMOTE_LE_PICK_UP, m_creature);

                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                DoScriptText(SAY_LE_THANKS, m_creature, pPlayer);
                                pPlayer->GroupEventHappens(QUEST_DIGGING_BONES, m_creature);
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

CreatureAI* GetAI_npc_letoll(Creature* pCreature)
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

/*######
## npc_mana_bomb_exp_trigger
######*/

enum
{
    SAY_COUNT_1                 = -1000472,
    SAY_COUNT_2                 = -1000473,
    SAY_COUNT_3                 = -1000474,
    SAY_COUNT_4                 = -1000475,
    SAY_COUNT_5                 = -1000476,

    SPELL_MANA_BOMB_LIGHTNING   = 37843,
    SPELL_MANA_BOMB_EXPL        = 35513,

    NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
    NPC_MANA_BOMB_KILL_TRIGGER  = 21039
};

struct npc_mana_bomb_exp_triggerAI : public ScriptedAI
{
    npc_mana_bomb_exp_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    GameObject* pManaBomb;

    bool m_bIsActivated;
    uint32 m_uiEventTimer;
    uint32 m_uiEventCounter;

    void Reset() override
    {
        pManaBomb = NULL;
        m_bIsActivated = false;
        m_uiEventTimer = 1000;
        m_uiEventCounter = 0;
    }

    void DoTrigger(Player* pPlayer, GameObject* pGo)
    {
        if (m_bIsActivated)
            return;

        m_bIsActivated = true;

        pPlayer->KilledMonsterCredit(NPC_MANA_BOMB_KILL_TRIGGER);

        pManaBomb = pGo;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bIsActivated)
            return;

        if (m_uiEventTimer < uiDiff)
        {
            m_uiEventTimer = 1000;

            if (m_uiEventCounter < 10)
                m_creature->CastSpell(m_creature, SPELL_MANA_BOMB_LIGHTNING, false);

            switch (m_uiEventCounter)
            {
                case 5:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_ACTIVE);

                    DoScriptText(SAY_COUNT_1, m_creature);
                    break;
                case 6:
                    DoScriptText(SAY_COUNT_2, m_creature);
                    break;
                case 7:
                    DoScriptText(SAY_COUNT_3, m_creature);
                    break;
                case 8:
                    DoScriptText(SAY_COUNT_4, m_creature);
                    break;
                case 9:
                    DoScriptText(SAY_COUNT_5, m_creature);
                    break;
                case 10:
                    m_creature->CastSpell(m_creature, SPELL_MANA_BOMB_EXPL, false);
                    break;
                case 30:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_READY);

                    Reset();
                    break;
            }

            ++m_uiEventCounter;
        }
        else
            m_uiEventTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mana_bomb_exp_trigger(Creature* pCreature)
{
    return new npc_mana_bomb_exp_triggerAI(pCreature);
}

/*######
## go_mana_bomb
######*/

bool GOUse_go_mana_bomb(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_MANA_BOMB_EXPL_TRIGGER, INTERACTION_DISTANCE))
    {
        if (npc_mana_bomb_exp_triggerAI* pBombAI = dynamic_cast<npc_mana_bomb_exp_triggerAI*>(pCreature->AI()))
            pBombAI->DoTrigger(pPlayer, pGo);
    }

    return true;
}

/*######
## go_veil_skith_cage & npc_captive_child
#####*/

enum
{
    QUEST_MISSING_FRIENDS     = 10852,
    NPC_CAPTIVE_CHILD         = 22314,
    SAY_THANKS_1              = -1000590,
    SAY_THANKS_2              = -1000591,
    SAY_THANKS_3              = -1000592,
    SAY_THANKS_4              = -1000593
};

bool GOUse_go_veil_skith_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
    {
        std::list<Creature*> lChildrenList;
        GetCreatureListWithEntryInGrid(lChildrenList, pGo, NPC_CAPTIVE_CHILD, INTERACTION_DISTANCE);
        for (std::list<Creature*>::const_iterator itr = lChildrenList.begin(); itr != lChildrenList.end(); ++itr)
        {
            pPlayer->KilledMonsterCredit(NPC_CAPTIVE_CHILD, (*itr)->GetObjectGuid());
            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_THANKS_1, *itr); break;
                case 1: DoScriptText(SAY_THANKS_2, *itr); break;
                case 2: DoScriptText(SAY_THANKS_3, *itr); break;
                case 3: DoScriptText(SAY_THANKS_4, *itr); break;
            }

            (*itr)->GetMotionMaster()->Clear();
            (*itr)->GetMotionMaster()->MovePoint(0, -2648.049f, 5274.573f, 1.691529f);
        }
    }
    return false;
};

struct npc_captive_child : public ScriptedAI
{
    npc_captive_child(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void MovementInform(uint32 uiMotionType, uint32 /*uiPointId*/) override
    {
        if (uiMotionType == POINT_MOTION_TYPE)
            m_creature->ForcedDespawn();                    // we only have one waypoint
    }
};

CreatureAI* GetAI_npc_captive_child(Creature* pCreature)
{
    return new npc_captive_child(pCreature);
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
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void JustStartedEscort() override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
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
                    pPlayer->GroupEventHappens(pPlayer->GetTeam() == ALLIANCE ? QUEST_ESCAPE_FROM_FIREWING_POINT_A : QUEST_ESCAPE_FROM_FIREWING_POINT_H, m_creature);
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

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEntanglingRootsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENTANGLING_ROOTS) == CAST_OK)
                m_uiEntanglingRootsTimer = urand(8000, 16000);
        }
        else
            m_uiEntanglingRootsTimer -= uiDiff;

        if (m_uiMoonfireTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOONFIRE) == CAST_OK)
                m_uiMoonfireTimer = urand(6000, 12000);
        }
        else
            m_uiMoonfireTimer -= uiDiff;

        if (m_uiWrathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WRATH) == CAST_OK)
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

CreatureAI* GetAI_npc_isla_starmane(Creature* pCreature)
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
                DoScriptText(SAY_SKYWING_TREE_DOWN , m_creature);
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
                m_creature->SummonCreature(NPC_LUANGA_THE_IMPRISONER, aLuangaSpawnCoords[0], aLuangaSpawnCoords[1], aLuangaSpawnCoords[2], 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                break;
            case 81:
                // Start transformation
                m_uiCycloneTimer = 100;
                break;
            case 82:
                DoScriptText(SAY_SKYWING_END, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_SKYWING, m_creature);
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

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
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

CreatureAI* GetAI_npc_skywing(Creature* pCreature)
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

CreatureAI* GetAI_npc_cenarion_sparrowhawk(Creature* pCreature)
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
};

struct npc_skyguard_prisonerAI : public npc_escortAI
{
    npc_skyguard_prisonerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            // ToDo: add additional WP when DB will support it
            if (m_creature->GetPositionZ() < 310.0f)
            {
                SetEscortPaused(true);
                //SetCurrentWaypoint(WP_ID_SPAWN_1);
                //SetEscortPaused(false);
                script_error_log("NPC entry %u, location %f, %f, %f does not have waypoints implemented for current spawn location. Please contact customer support!", m_creature->GetEntry(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            }
            else if (m_creature->GetPositionZ() < 330.0f)
            {
                SetEscortPaused(true);
                //SetCurrentWaypoint(WP_ID_SPAWN_2);
                //SetEscortPaused(false);
                script_error_log("NPC entry %u, location %f, %f, %f does not have waypoints implemented for current spawn location. Please contact customer support!", m_creature->GetEntry(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            }
            // else just use standard WP

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
            case 0:
                DoScriptText(SAY_ESCORT_START, m_creature);
                break;
            case 13:
                m_creature->SummonCreature(NPC_WING_GUARD, -4179.043f, 3081.007f, 328.28f, 4.51f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_WING_GUARD, -4181.610f, 3081.289f, 328.32f, 4.52f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 14:
                DoScriptText(SAY_AMBUSH_END, m_creature);
                break;
            case 18:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                SetRun();

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ID_ESCAPE_SKETTIS, m_creature);
                break;
        }
    }
};

CreatureAI* GetAI_npc_skyguard_prisoner(Creature* pCreature)
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

void AddSC_terokkar_forest()
{
    Script* pNewScript;

    pNewScript = new Script;
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
    pNewScript->Name = "npc_mana_bomb_exp_trigger";
    pNewScript->GetAI = &GetAI_npc_mana_bomb_exp_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mana_bomb";
    pNewScript->pGOUse = &GOUse_go_mana_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_veil_skith_cage";
    pNewScript->pGOUse = &GOUse_go_veil_skith_cage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_captive_child";
    pNewScript->GetAI = &GetAI_npc_captive_child;
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
}
