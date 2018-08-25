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
SDName: Dustwallow_Marsh
SD%Complete: 95
SDComment: Quest support: 1173, 1222, 1270, 1273, 1324, 11180, 11198, 11209.
SDCategory: Dustwallow Marsh
EndScriptData */

/* ContentData
mobs_risen_husk_spirit
npc_restless_apparition
npc_morokk
npc_ogron
npc_private_hendel
npc_stinky_ignatz
at_nats_landing
boss_tethyr
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "Entities/TemporarySpawn.h"

/*######
## mobs_risen_husk_spirit
######*/

enum
{
    QUEST_WHATS_HAUNTING_WITCH_HILL  = 11180,
    SPELL_SUMMON_RESTLESS_APPARITION = 42511,
    SPELL_CONSUME_FLESH              = 37933,               // Risen Husk
    SPELL_INTANGIBLE_PRESENCE        = 43127,               // Risen Spirit
    NPC_RISEN_HUSK                   = 23555,
    NPC_RISEN_SPIRIT                 = 23554
};

struct mobs_risen_husk_spiritAI : public ScriptedAI
{
    mobs_risen_husk_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiConsumeFlesh_Timer;
    uint32 m_uiIntangiblePresence_Timer;

    Player* m_pCreditPlayer;

    void Reset() override
    {
        m_uiConsumeFlesh_Timer = 10000;
        m_uiIntangiblePresence_Timer = 5000;

        m_pCreditPlayer = nullptr;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_pCreditPlayer)
            m_pCreditPlayer->RewardPlayerAndGroupAtEventCredit(pSummoned->GetEntry(), pSummoned);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        if (Player* pPlayer = pDoneBy->GetBeneficiaryPlayer())
        {
            if (pPlayer->GetQuestStatus(QUEST_WHATS_HAUNTING_WITCH_HILL) == QUEST_STATUS_INCOMPLETE)
            {
                m_pCreditPlayer = pPlayer;
                m_creature->CastSpell(pDoneBy, SPELL_SUMMON_RESTLESS_APPARITION, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiConsumeFlesh_Timer < uiDiff)
        {
            if (m_creature->GetEntry() == NPC_RISEN_HUSK)
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONSUME_FLESH);

            m_uiConsumeFlesh_Timer = 15000;
        }
        else
            m_uiConsumeFlesh_Timer -= uiDiff;

        if (m_uiIntangiblePresence_Timer < uiDiff)
        {
            if (m_creature->GetEntry() == NPC_RISEN_SPIRIT)
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTANGIBLE_PRESENCE);

            m_uiIntangiblePresence_Timer = 20000;
        }
        else
            m_uiIntangiblePresence_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mobs_risen_husk_spirit(Creature* pCreature)
{
    return new mobs_risen_husk_spiritAI(pCreature);
}

/*######
## npc_restless_apparition
######*/

enum
{
    SAY_RAND_1      = -1000543,
    SAY_RAND_2      = -1000544,
    SAY_RAND_3      = -1000545,
    SAY_RAND_4      = -1000546,
    SAY_RAND_5      = -1000547,
    SAY_RAND_6      = -1000548,
    SAY_RAND_7      = -1000549,
    SAY_RAND_8      = -1000550
};

struct npc_restless_apparitionAI : public ScriptedAI
{
    npc_restless_apparitionAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiTalk_Timer;

    void Reset() override
    {
        m_uiTalk_Timer = 1000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_uiTalk_Timer)
            return;

        if (m_uiTalk_Timer <= uiDiff)
        {
            switch (urand(0, 7))
            {
                case 0: DoScriptText(SAY_RAND_1, m_creature); break;
                case 1: DoScriptText(SAY_RAND_2, m_creature); break;
                case 2: DoScriptText(SAY_RAND_3, m_creature); break;
                case 3: DoScriptText(SAY_RAND_4, m_creature); break;
                case 4: DoScriptText(SAY_RAND_5, m_creature); break;
                case 5: DoScriptText(SAY_RAND_6, m_creature); break;
                case 6: DoScriptText(SAY_RAND_7, m_creature); break;
                case 7: DoScriptText(SAY_RAND_8, m_creature); break;
            }

            m_uiTalk_Timer = 0;
        }
        else
            m_uiTalk_Timer -= uiDiff;
    }
};

UnitAI* GetAI_npc_restless_apparition(Creature* pCreature)
{
    return new npc_restless_apparitionAI(pCreature);
}

/*######
## npc_morokk
######*/

enum
{
    SAY_MOR_CHALLENGE       = -1000499,
    SAY_MOR_SCARED          = -1000500,

    NPC_MOROKK              = 4500,
    QUEST_CHALLENGE_MOROKK  = 1173,

    FACTION_MOR_HOSTILE     = 168,
    FACTION_MOR_RUNNING     = 35,
    FACTION_MOR_SPAWN       = 29
};

struct npc_morokkAI : public npc_escortAI
{
    npc_morokkAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bIsSuccess = false;
        Reset();
    }

    bool m_bIsSuccess;

    void Reset() override {}

    void JustRespawned() override
    {
        npc_escortAI::JustRespawned();
        m_creature->setFaction(FACTION_MOR_SPAWN);
    }

    void JustReachedHome() override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (!m_bIsSuccess)
            {
                FailQuestForPlayerAndGroup();
                m_creature->ForcedDespawn();
            }
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                SetEscortPaused(true);
                break;
        }
    }

    void UpdateEscortAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    if (pPlayer->isAlive() && pPlayer->IsInRange(m_creature, 0, 70))
                    {
                        m_bIsSuccess = false;
                        DoScriptText(SAY_MOR_CHALLENGE, m_creature, pPlayer);
                        m_creature->SetImmuneToPlayer(false);
                        SetReactState(REACT_AGGRESSIVE);
                        m_creature->setFaction(FACTION_MOR_HOSTILE);
                        AttackStart(pPlayer);
                    }
                    else
                        SetEscortPaused(false);
                }
                else
                    SetEscortPaused(false);
            }
            return;
        }
        if (m_creature->GetHealthPercent() <= 30.0f)
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_CHALLENGE_MOROKK, m_creature);
                    m_creature->setFaction(FACTION_MOR_RUNNING);
                    m_bIsSuccess = true;
                    m_creature->RemoveAllAurasOnEvade();
                    m_creature->CombatStop(true);
                    m_creature->SetImmuneToPlayer(true);
                    SetEscortPaused(false);
                    SetReactState(REACT_PASSIVE);
                    DoScriptText(SAY_MOR_SCARED, m_creature);
                    m_creature->GetMotionMaster()->Clear(false); // TODO: make whole EscortAI work like this
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_morokk(Creature* pCreature)
{
    return new npc_morokkAI(pCreature);
}

bool QuestAccept_npc_morokk(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CHALLENGE_MOROKK)
    {
        if (npc_morokkAI* pEscortAI = dynamic_cast<npc_morokkAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer, pQuest);

        return true;
    }

    return false;
}

/*######
## npc_ogron
######*/

enum
{
    SAY_OGR_START                       = -1000452,
    SAY_OGR_SPOT                        = -1000453,
    SAY_OGR_RET_WHAT                    = -1000454,
    SAY_OGR_RET_SWEAR                   = -1000455,
    SAY_OGR_REPLY_RET                   = -1000456,
    SAY_OGR_RET_TAKEN                   = -1000457,
    SAY_OGR_TELL_FIRE                   = -1000458,
    SAY_OGR_RET_NOCLOSER                = -1000459,
    SAY_OGR_RET_NOFIRE                  = -1000460,
    SAY_OGR_RET_HEAR                    = -1000461,
    SAY_OGR_CAL_FOUND                   = -1000462,
    SAY_OGR_CAL_MERCY                   = -1000463,
    SAY_OGR_HALL_GLAD                   = -1000464,
    EMOTE_OGR_RET_ARROW                 = -1000465,
    SAY_OGR_RET_ARROW                   = -1000466,
    SAY_OGR_CAL_CLEANUP                 = -1000467,
    SAY_OGR_NODIE                       = -1000468,
    SAY_OGR_SURVIVE                     = -1000469,
    SAY_OGR_RET_LUCKY                   = -1000470,
    SAY_OGR_THANKS                      = -1000471,

    QUEST_QUESTIONING                   = 1273,

    FACTION_GENERIC_FRIENDLY            = 35,
    FACTION_THER_HOSTILE                = 151,

    NPC_REETHE                          = 4980,
    NPC_CALDWELL                        = 5046,
    NPC_HALLAN                          = 5045,
    NPC_SKIRMISHER                      = 5044,

    SPELL_FAKE_SHOT                     = 7105,

    PHASE_INTRO                         = 0,
    PHASE_GUESTS                        = 1,
    PHASE_FIGHT                         = 2,
    PHASE_COMPLETE                      = 3
};

static float m_afSpawn[] = { -3383.501953f, -3203.383301f, 36.149f};
static float m_afMoveTo[] = { -3371.414795f, -3212.179932f, 34.210f};

struct npc_ogronAI : public npc_escortAI
{
    npc_ogronAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        lCreatureList.clear();
        m_uiPhase = 0;
        m_uiPhaseCounter = 0;
        Reset();
    }

    std::list<Creature*> lCreatureList;

    uint32 m_uiPhase;
    uint32 m_uiPhaseCounter;
    uint32 m_uiGlobalTimer;

    void Reset() override
    {
        m_uiGlobalTimer = 5000;

        if (HasEscortState(STATE_ESCORT_PAUSED) && m_uiPhase == PHASE_FIGHT)
            m_uiPhase = PHASE_COMPLETE;

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            lCreatureList.clear();
            m_uiPhase = 0;
            m_uiPhaseCounter = 0;
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) && pWho->GetEntry() == NPC_REETHE && lCreatureList.empty())
            lCreatureList.push_back((Creature*)pWho);

        npc_escortAI::MoveInLineOfSight(pWho);
    }

    Creature* GetCreature(uint32 uiCreatureEntry)
    {
        if (!lCreatureList.empty())
        {
            for (auto& itr : lCreatureList)
            {
                if (itr->GetEntry() == uiCreatureEntry && itr->isAlive())
                    return itr;
            }
        }

        return nullptr;
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 9:
                DoScriptText(SAY_OGR_SPOT, m_creature);
                break;
            case 10:
                if (Creature* pReethe = GetCreature(NPC_REETHE))
                    DoScriptText(SAY_OGR_RET_WHAT, pReethe);
                break;
            case 11:
                SetEscortPaused(true);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        lCreatureList.push_back(pSummoned);

        pSummoned->setFaction(FACTION_GENERIC_FRIENDLY);

        if (pSummoned->GetEntry() == NPC_CALDWELL)
            pSummoned->GetMotionMaster()->MovePoint(0, m_afMoveTo[0], m_afMoveTo[1], m_afMoveTo[2]);
        else
        {
            if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
            {
                // will this conversion work without compile warning/error?
                size_t iSize = lCreatureList.size();
                pSummoned->GetMotionMaster()->MoveFollow(pCaldwell, 0.5f, (M_PI / 2) * (int)iSize);
            }
        }
    }

    void DoStartAttackMe()
    {
        if (!lCreatureList.empty())
        {
            for (auto& itr : lCreatureList)
            {
                if (itr->GetEntry() == NPC_REETHE)
                    continue;

                if (itr->isAlive())
                {
                    itr->setFaction(FACTION_THER_HOSTILE);
                    itr->AI()->AttackStart(m_creature);
                }
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiGlobalTimer < uiDiff)
                {
                    m_uiGlobalTimer = 5000;

                    switch (m_uiPhase)
                    {
                        case PHASE_INTRO:
                        {
                            switch (m_uiPhaseCounter)
                            {
                                case 0:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_SWEAR, pReethe);
                                    break;
                                case 1:
                                    DoScriptText(SAY_OGR_REPLY_RET, m_creature);
                                    break;
                                case 2:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_TAKEN, pReethe);
                                    break;
                                case 3:
                                    DoScriptText(SAY_OGR_TELL_FIRE, m_creature);
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_NOCLOSER, pReethe);
                                    break;
                                case 4:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_NOFIRE, pReethe);
                                    break;
                                case 5:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_HEAR, pReethe);

                                    m_creature->SummonCreature(NPC_CALDWELL, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_HALLAN, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_SKIRMISHER, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_SKIRMISHER, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000);

                                    m_uiPhase = PHASE_GUESTS;
                                    break;
                            }
                            break;
                        }
                        case PHASE_GUESTS:
                        {
                            switch (m_uiPhaseCounter)
                            {
                                case 6:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_FOUND, pCaldwell);
                                    break;
                                case 7:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_MERCY, pCaldwell);
                                    break;
                                case 8:
                                    if (Creature* pHallan = GetCreature(NPC_HALLAN))
                                    {
                                        DoScriptText(SAY_OGR_HALL_GLAD, pHallan);

                                        if (Creature* pReethe = GetCreature(NPC_REETHE))
                                            pHallan->CastSpell(pReethe, SPELL_FAKE_SHOT, TRIGGERED_NONE);
                                    }
                                    break;
                                case 9:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                    {
                                        DoScriptText(EMOTE_OGR_RET_ARROW, pReethe);
                                        DoScriptText(SAY_OGR_RET_ARROW, pReethe);
                                    }
                                    break;
                                case 10:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_CLEANUP, pCaldwell);

                                    DoScriptText(SAY_OGR_NODIE, m_creature);
                                    break;
                                case 11:
                                    DoStartAttackMe();
                                    m_uiPhase = PHASE_FIGHT;
                                    break;
                            }
                            break;
                        }
                        case PHASE_COMPLETE:
                        {
                            switch (m_uiPhaseCounter)
                            {
                                case 12:
                                    if (Player* pPlayer = GetPlayerForEscort())
                                        pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_QUESTIONING, m_creature);

                                    DoScriptText(SAY_OGR_SURVIVE, m_creature);
                                    break;
                                case 13:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_LUCKY, pReethe);
                                    break;
                                case 14:
                                    DoScriptText(SAY_OGR_THANKS, m_creature);
                                    SetRun();
                                    SetEscortPaused(false);
                                    break;
                            }
                            break;
                        }
                    }

                    if (m_uiPhase != PHASE_FIGHT)
                        ++m_uiPhaseCounter;
                }
                else
                    m_uiGlobalTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_ogron(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_QUESTIONING)
    {
        if (npc_ogronAI* pEscortAI = dynamic_cast<npc_ogronAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest, true);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
            DoScriptText(SAY_OGR_START, pCreature, pPlayer);
        }
    }

    return true;
}

UnitAI* GetAI_npc_ogron(Creature* pCreature)
{
    return new npc_ogronAI(pCreature);
}

/*######
## npc_private_hendel
######*/

enum
{
    EMOTE_SURRENDER             = -1000415,

    QUEST_MISSING_DIPLO_PT16    = 1324,
    FACTION_HOSTILE             = 168,                      // guessed, may be different

    NPC_SENTRY                  = 5184,                     // helps hendel
    NPC_JAINA                   = 4968,                     // appears once hendel gives up
    NPC_TERVOSH                 = 4967,
    NPC_PAINED                  = 4965,

    SPELL_TELEPORT_VISUAL       = 12980,
    SPELL_TELEPORT              = 7079
};

struct Location
{
    float fX, fY, fZ, fO;
    float fDestX, fDestY, fDestZ;
    uint32 uiEntry;
};

const Location lOutroSpawns[] =
{
    {
        -2857.604492f, -3354.784912f, 35.369640f, 3.16604f,
        -2881.546631f, -3346.477539f, 34.143719f,
        NPC_TERVOSH
    },
    {
        -2858.120117f, -3358.469971f, 36.086300f, 3.16604f,
        -2879.697998f, -3347.789063f, 34.772892f,
        NPC_JAINA
    },
    {
        -2857.379883f, -3351.370117f, 34.178001f, 3.16604f,
        -2879.959961f, -3344.469971f, 34.670502f,
        NPC_PAINED
    }
};

const float fSentryFleePoint[] {-2917.56f, -3329.90f, 30.37f};

struct npc_private_hendelAI : public ScriptedAI
{
    npc_private_hendelAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    ObjectGuid guidPlayer;

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->getVictim())
            return;

        if (!m_creature->CanAttackNow(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // If Private Hendel is hit by spell Teleport (from DBScript)
        // this means it is time to grant quest credit to the player previously stored to this intend
        if (pSpell->Id == SPELL_TELEPORT)
        {
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(guidPlayer))
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_MISSING_DIPLO_PT16, m_creature);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (uiDamage > m_creature->GetHealth() || m_creature->GetHealthPercent() < 20.0f)
        {
            if (Player* pPlayer = pDoneBy->GetBeneficiaryPlayer())
            {
                if (pPlayer->GetQuestStatus(QUEST_MISSING_DIPLO_PT16) == QUEST_STATUS_INCOMPLETE)
                    guidPlayer = pPlayer->GetObjectGuid();  // Store the player to give quest credit later
            }

            uiDamage = 0;

            DoScriptText(EMOTE_SURRENDER, m_creature);
            EnterEvadeMode();

            // Make the two sentries flee and despawn
            std::list<Creature*> lSentryList;
            GetCreatureListWithEntryInGrid(lSentryList, m_creature, NPC_SENTRY, 40.0f);

            for (std::list<Creature*>::const_iterator itr = lSentryList.begin(); itr != lSentryList.end(); ++itr)
            {
                if ((*itr)->isAlive())
                {
                    (*itr)->RemoveAllAurasOnEvade();
                    (*itr)->CombatStop(true);
                    (*itr)->SetWalk(false);
                    (*itr)->GetMotionMaster()->MovePoint(0, fSentryFleePoint[0], fSentryFleePoint[1], fSentryFleePoint[2]);
                    (*itr)->ForcedDespawn(4000);
                }
            }

            // Summon Jaina Proudmoore, Archmage Tervosh and Pained
            for (const auto& lOutroSpawn : lOutroSpawns)
            {
                Creature* pCreature = m_creature->SummonCreature(lOutroSpawn.uiEntry, lOutroSpawn.fX, lOutroSpawn.fY, lOutroSpawn.fZ, lOutroSpawn.fO, TEMPSPAWN_TIMED_DESPAWN, 3 * MINUTE * IN_MILLISECONDS, false, true);
                if (pCreature)
                {
                    pCreature->CastSpell(pCreature, SPELL_TELEPORT_VISUAL, TRIGGERED_NONE);
                    pCreature->GetMotionMaster()->MovePoint(0, lOutroSpawn.fDestX, lOutroSpawn.fDestY, lOutroSpawn.fDestZ);

                    // Exception case for Archmage Tervosh: the outro event is a simple speech with visual spell cast
                    // so it will be handled by a DBScript held by NPC Archmage Tervosh
                    if (pCreature->GetEntry() == NPC_TERVOSH)
                    {
                        // Remove Gossip and Quest Giver flag from now, they will be re-added later to Archmage Tervosh in DBScript
                        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                        // The DBScript will be done here
                        pCreature->GetMotionMaster()->MoveWaypoint(0);
                    }
                }
            }
        }
    }
};

bool QuestAccept_npc_private_hendel(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT16)
    {
        pCreature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);
        pCreature->AI()->AttackStart(pPlayer);

        // Find the nearby sentries in order to make them attack
        // The two sentries are linked to Private Hendel in DB to ensure they respawn together
        std::list<Creature*> lSentryList;
        GetCreatureListWithEntryInGrid(lSentryList, pCreature, NPC_SENTRY, 40.0f);

        for (std::list<Creature*>::const_iterator itr = lSentryList.begin(); itr != lSentryList.end(); ++itr)
        {
            if ((*itr)->isAlive())
            {
                (*itr)->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);
                (*itr)->AI()->AttackStart(pPlayer);
            }
        }
    }

    return true;
}

UnitAI* GetAI_npc_private_hendel(Creature* pCreature)
{
    return new npc_private_hendelAI(pCreature);
}

/*#####
## npc_stinky_ignatz
## TODO Note: Faction change is guessed
#####*/

enum
{
    QUEST_ID_STINKYS_ESCAPE_ALLIANCE    = 1222,
    QUEST_ID_STINKYS_ESCAPE_HORDE       = 1270,

    SAY_STINKY_BEGIN                    = -1000958,
    SAY_STINKY_FIRST_STOP               = -1000959,
    SAY_STINKY_SECOND_STOP              = -1001141,
    SAY_STINKY_THIRD_STOP_1             = -1001142,
    SAY_STINKY_THIRD_STOP_2             = -1001143,
    SAY_STINKY_THIRD_STOP_3             = -1001144,
    SAY_STINKY_PLANT_GATHERED           = -1001145,
    SAY_STINKY_END                      = -1000962,
    SAY_STINKY_AGGRO_1                  = -1000960,
    SAY_STINKY_AGGRO_2                  = -1000961,
    SAY_STINKY_AGGRO_3                  = -1001146,
    SAY_STINKY_AGGRO_4                  = -1001147,

    GO_BOGBEAN_PLANT                    = 20939,
};

struct npc_stinky_ignatzAI : public npc_escortAI
{
    npc_stinky_ignatzAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_bogbeanPlantGuid;

    void Reset() override {}

    void Aggro(Unit* pWho) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_STINKY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_STINKY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_STINKY_AGGRO_3, m_creature); break;
            case 3: DoScriptText(SAY_STINKY_AGGRO_4, m_creature, pWho); break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_STINKY_BEGIN, m_creature);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 5:
                DoScriptText(SAY_STINKY_FIRST_STOP, m_creature);
                break;
            case 10:
                DoScriptText(SAY_STINKY_SECOND_STOP, m_creature);
                break;
            case 24:
                DoScriptText(SAY_STINKY_THIRD_STOP_1, m_creature);
                break;
            case 25:
                DoScriptText(SAY_STINKY_THIRD_STOP_2, m_creature);
                if (GameObject* pBogbeanPlant = GetClosestGameObjectWithEntry(m_creature, GO_BOGBEAN_PLANT, DEFAULT_VISIBILITY_DISTANCE))
                {
                    m_bogbeanPlantGuid = pBogbeanPlant->GetObjectGuid();
                    m_creature->SetFacingToObject(pBogbeanPlant);
                }
                break;
            case 26:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_STINKY_THIRD_STOP_3, m_creature, pPlayer);
                break;
            case 29:
                m_creature->HandleEmote(EMOTE_STATE_USESTANDING);
                break;
            case 30:
                DoScriptText(SAY_STINKY_PLANT_GATHERED, m_creature);
                break;
            case 39:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->RewardPlayerAndGroupAtEventExplored(pPlayer->GetTeam() == ALLIANCE ? QUEST_ID_STINKYS_ESCAPE_ALLIANCE : QUEST_ID_STINKYS_ESCAPE_HORDE, m_creature);
                    DoScriptText(SAY_STINKY_END, m_creature, pPlayer);
                }
                break;
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        if (uiPointId == 30)
        {
            if (GameObject* pBogbeanPlant = m_creature->GetMap()->GetGameObject(m_bogbeanPlantGuid))
                pBogbeanPlant->Use(m_creature);
            m_bogbeanPlantGuid.Clear();
            m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
        }
    }
};

UnitAI* GetAI_npc_stinky_ignatz(Creature* pCreature)
{
    return new npc_stinky_ignatzAI(pCreature);
}

bool QuestAccept_npc_stinky_ignatz(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_STINKYS_ESCAPE_ALLIANCE || pQuest->GetQuestId() == QUEST_ID_STINKYS_ESCAPE_HORDE)
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());

    return true;
}

/*######
## at_nats_landing
######*/
enum
{
    QUEST_NATS_BARGAIN = 11209,
    SPELL_FISH_PASTE   = 42644,
    NPC_LURKING_SHARK  = 23928
};

bool AreaTrigger_at_nats_landing(Player* pPlayer, const AreaTriggerEntry* /*pAt*/)
{
    if (pPlayer->GetQuestStatus(QUEST_NATS_BARGAIN) == QUEST_STATUS_INCOMPLETE && pPlayer->HasAura(SPELL_FISH_PASTE))
    {
        Creature* pShark = GetClosestCreatureWithEntry(pPlayer, NPC_LURKING_SHARK, 20.0f);

        if (!pShark)
            pShark = pPlayer->SummonCreature(NPC_LURKING_SHARK, -4246.243f, -3922.356f, -7.488f, 5.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 100000);

        pShark->AI()->AttackStart(pPlayer);
        return false;
    }
    return true;
}

/*######
## boss_tethyr
######*/

enum
{
    SPELL_WATER_BOLT            = 42574,
    SPELL_SPOUT_LEFT            = 42581,                // triggers 42584
    SPELL_SPOUT_RIGHT           = 42582,
    SPELL_CANNON_BLAST          = 42578,                // triggers 42576
    SPELL_CANNON_BLAST_DMG      = 42576,

    NPC_TETHYR                  = 23899,
    NPC_THERAMORE_MARKSMAN      = 23900,
    NPC_THERAMORE_CANNON        = 23907,

    GO_COVE_CANNON              = 186432,               // cast 42578
    QUEST_ID_TETHYR             = 11198,

    WORLD_STATE_TETHYR_SHOW     = 3083,
    WORLD_STATE_TETHYR_COUNT    = 3082,

    MAX_MARKSMEN                = 12,
    PHASE_NORMAL                = 1,
    PHASE_SPOUT                 = 2,
};

struct boss_tethyrAI : public Scripted_NoMovementAI
{
    boss_tethyrAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        // send world states to player summoner
        if (m_creature->IsTemporarySummon())
            m_summonerGuid = m_creature->GetSpawnerGuid();

        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_summonerGuid))
        {
            pPlayer->SendUpdateWorldState(WORLD_STATE_TETHYR_SHOW, 1);
            pPlayer->SendUpdateWorldState(WORLD_STATE_TETHYR_COUNT, MAX_MARKSMEN);
        }

        m_creature->SetSwim(true);
        Reset();
    }

    ObjectGuid m_summonerGuid;

    uint8 m_uiPhase;
    uint8 m_uiMarksmenKilled;
    uint32 m_uiWaterBoltTimer;
    uint32 m_uiSpoutEndTimer;

    void Reset() override
    {
        m_uiPhase           = PHASE_NORMAL;
        m_uiMarksmenKilled  = 0;
        m_uiWaterBoltTimer  = urand(0, 1000);
        m_uiSpoutEndTimer   = 7000;
    }

    void JustReachedHome() override
    {
        // cleanup
        DoEncounterCleanup();
        m_creature->ForcedDespawn(5000);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        // quest complete and cleanup
        if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_summonerGuid))
            pSummoner->RewardPlayerAndGroupAtEventExplored(QUEST_ID_TETHYR, m_creature);

        // ToDo: trigger some fireworks!
        DoEncounterCleanup();
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType == WAYPOINT_MOTION_TYPE)
        {
            // start attacking
            if (uiPointId == 12)
            {
                // make cannons usable
                std::list<GameObject*> lCannonsInRange;
                GetGameObjectListWithEntryInGrid(lCannonsInRange, m_creature, GO_COVE_CANNON, 100.0f);

                for (std::list<GameObject*>::const_iterator itr = lCannonsInRange.begin(); itr != lCannonsInRange.end(); ++itr)
                    (*itr)->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

                // attack all marksmen
                std::list<Creature*> lMarksmenInRange;
                GetCreatureListWithEntryInGrid(lMarksmenInRange, m_creature, NPC_THERAMORE_MARKSMAN, 100.0f);

                for (std::list<Creature*>::const_iterator itr = lMarksmenInRange.begin(); itr != lMarksmenInRange.end(); ++itr)
                {
                    (*itr)->AI()->AttackStart(m_creature);
                    AttackStart(*itr);
                }
            }
        }
        else if (uiMotionType == POINT_MOTION_TYPE)
        {
            // Spout on cannon point reach
            if (uiPointId)
            {
                if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_SPOUT_LEFT : SPELL_SPOUT_RIGHT, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    // Remove the target focus
                    m_creature->SetTarget(nullptr);
                    m_uiPhase = PHASE_SPOUT;
                }
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        // count the marksmen
        if (pVictim->GetEntry() != NPC_THERAMORE_MARKSMAN)
            return;

        ++m_uiMarksmenKilled;

        // update world state
        if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_summonerGuid))
        {
            pSummoner->SendUpdateWorldState(WORLD_STATE_TETHYR_COUNT, MAX_MARKSMEN - m_uiMarksmenKilled);

            // fail quest if all marksmen are killed
            if (m_uiMarksmenKilled == MAX_MARKSMEN)
            {
                pSummoner->FailQuest(QUEST_ID_TETHYR);
                EnterEvadeMode();
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // spout on cannon
        if (pCaster->GetEntry() == NPC_THERAMORE_CANNON && pSpell->Id == SPELL_CANNON_BLAST_DMG)
        {
            if (m_uiPhase == PHASE_SPOUT)
                return;

            // not all cannons have same distance range
            uint8 uiDistMod = pCaster->GetPositionY() > -4650.0f ? 6 : 5;

            float fX, fY, fZ;
            pCaster->GetContactPoint(m_creature, fX, fY, fZ, uiDistMod * ATTACK_DISTANCE);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, m_creature->GetPositionZ());

            m_uiWaterBoltTimer = 10000;
        }
    }

    // function to cleanup the world states and GO flags
    void DoEncounterCleanup()
    {
        // remove world state
        if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_summonerGuid))
            pSummoner->SendUpdateWorldState(WORLD_STATE_TETHYR_SHOW, 0);

        // reset all cannons
        std::list<GameObject*> lCannonsInRange;
        GetGameObjectListWithEntryInGrid(lCannonsInRange, m_creature, GO_COVE_CANNON, 100.0f);

        for (std::list<GameObject*>::const_iterator itr = lCannonsInRange.begin(); itr != lCannonsInRange.end(); ++itr)
            (*itr)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

        // despawn all marksmen
        std::list<Creature*> lMarksmenInRange;
        GetCreatureListWithEntryInGrid(lMarksmenInRange, m_creature, NPC_THERAMORE_MARKSMAN, 100.0f);

        for (std::list<Creature*>::const_iterator itr = lMarksmenInRange.begin(); itr != lMarksmenInRange.end(); ++itr)
            (*itr)->ForcedDespawn(30000);
    }

    // Custom threat management
    bool SelectCustomHostileTarget()
    {
        // Not started combat or evading prevented
        if (!m_creature->isInCombat() || m_creature->HasAuraType(SPELL_AURA_MOD_TAUNT))
            return false;

        // Check if there are still enemies (marksmen) in the threatList
        ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
        for (auto itr : threatList)
        {
            if (itr->getUnitGuid().IsCreature())
                return true;
        }

        EnterEvadeMode();
        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!SelectCustomHostileTarget())
            return;

        if (m_uiPhase == PHASE_SPOUT)
        {
            if (m_uiSpoutEndTimer < uiDiff)
            {
                // Remove rotation auras
                m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_LEFT);
                m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_RIGHT);

                m_uiPhase = PHASE_NORMAL;
                m_uiSpoutEndTimer = 7000;
                m_uiWaterBoltTimer = urand(0, 1000);
            }
            else
                m_uiSpoutEndTimer -= uiDiff;
        }
        else if (m_uiPhase == PHASE_NORMAL)
        {
            if (m_uiWaterBoltTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_WATER_BOLT) == CAST_OK)
                    {
                        // mimic boss turning because of the missing threat system
                        m_creature->SetTarget(pTarget);
                        m_creature->SetInFront(pTarget);

                        m_uiWaterBoltTimer = urand(0, 1000);
                    }
                }
            }
            else
                m_uiWaterBoltTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_tethyr(Creature* pCreature)
{
    return new boss_tethyrAI(pCreature);
}

/*######
## npc_mottled_drywallow_crocolisks
######*/

enum
{
    QUEST_THE_GRIMTOTEM_WEAPON = 11169,
    SPELL_CAPTURED_CREDIT      = 42455,
    AURA_CAPTURED_TOTEM        = 42454,
    NPC_CAPTURED_TOTEM         = 23811
};

struct npc_mottled_drywallow_crocoliskAI : public ScriptedAI
{
    npc_mottled_drywallow_crocoliskAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void JustDied(Unit* pVictim) override
    {
        if (Unit* totem = GetClosestCreatureWithEntry(m_creature, NPC_CAPTURED_TOTEM, 8.0f))
        {
            if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER &&
                    ((Player*)pVictim)->GetQuestStatus(QUEST_THE_GRIMTOTEM_WEAPON) == QUEST_STATUS_INCOMPLETE)
            {
                totem->CastSpell(pVictim, SPELL_CAPTURED_CREDIT, TRIGGERED_NONE);
                ((Player*)pVictim)->KilledMonsterCredit(NPC_CAPTURED_TOTEM);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## at_sentry_point
######*/

enum SentryPoint
{
    QUEST_MISSING_DIPLO_PT14    = 1265,
    SPELL_TELEPORT_VISUAL_2     = 799,  // TODO Find the correct spell
    NPC_SENTRY_POINT_GUARD      = 5085
};

bool AreaTrigger_at_sentry_point(Player* pPlayer, const AreaTriggerEntry* /*pAt*/)
{
    QuestStatus quest_status = pPlayer->GetQuestStatus(QUEST_MISSING_DIPLO_PT14);
    if (pPlayer->isDead() || quest_status == QUEST_STATUS_NONE || quest_status == QUEST_STATUS_COMPLETE)
        return false;

    if (!GetClosestCreatureWithEntry(pPlayer, NPC_TERVOSH, 100.0f))
    {
        if (Creature* pTervosh = pPlayer->SummonCreature(NPC_TERVOSH, -3476.51f, -4105.94f, 17.1f, 5.3816f, TEMPSPAWN_TIMED_DESPAWN, 60000))
        {
            pTervosh->CastSpell(pTervosh, SPELL_TELEPORT_VISUAL_2, TRIGGERED_OLD_TRIGGERED);

            if (Creature* pGuard = GetClosestCreatureWithEntry(pTervosh, NPC_SENTRY_POINT_GUARD, 15.0f))
            {
                pGuard->SetFacingToObject(pTervosh);
                pGuard->HandleEmote(EMOTE_ONESHOT_SALUTE);
            }
        }
    }

    return true;
};

UnitAI* GetAI_npc_mottled_drywallow_crocolisk(Creature* pCreature)
{
    return new npc_mottled_drywallow_crocoliskAI(pCreature);
}

void AddSC_dustwallow_marsh()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mobs_risen_husk_spirit";
    pNewScript->GetAI = &GetAI_mobs_risen_husk_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_restless_apparition";
    pNewScript->GetAI = &GetAI_npc_restless_apparition;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_morokk";
    pNewScript->GetAI = &GetAI_npc_morokk;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_morokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ogron";
    pNewScript->GetAI = &GetAI_npc_ogron;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_ogron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_private_hendel";
    pNewScript->GetAI = &GetAI_npc_private_hendel;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_private_hendel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stinky_ignatz";
    pNewScript->GetAI = &GetAI_npc_stinky_ignatz;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_stinky_ignatz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_nats_landing";
    pNewScript->pAreaTrigger = &AreaTrigger_at_nats_landing;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_sentry_point";
    pNewScript->pAreaTrigger = &AreaTrigger_at_sentry_point;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_tethyr";
    pNewScript->GetAI = &GetAI_boss_tethyr;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mottled_drywallow_crocolisk";
    pNewScript->GetAI = &GetAI_npc_mottled_drywallow_crocolisk;
    pNewScript->RegisterSelf();
}
