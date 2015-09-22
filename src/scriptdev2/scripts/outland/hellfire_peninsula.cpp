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
SDName: Hellfire_Peninsula
SD%Complete: 100
SDComment: Quest support: 9375, 9410, 9418, 10286, 10629, 10838, 10935.
SDCategory: Hellfire Peninsula
EndScriptData */

/* ContentData
npc_aeranas
npc_ancestral_wolf
npc_demoniac_scryer
npc_wounded_blood_elf
npc_fel_guard_hound
npc_anchorite_barada
npc_colonel_jules
npc_magister_aledis
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "pet_ai.h"

/*######
## npc_aeranas
######*/

enum
{
    SAY_SUMMON                      = -1000138,
    SAY_FREE                        = -1000139,

    FACTION_HOSTILE                 = 16,
    FACTION_FRIENDLY                = 35,

    SPELL_ENVELOPING_WINDS          = 15535,
    SPELL_SHOCK                     = 12553,
};

struct npc_aeranasAI : public ScriptedAI
{
    npc_aeranasAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiFactionTimer;
    uint32 m_uiEnvelopingWindsTimer;
    uint32 m_uiShockTimer;

    void Reset() override
    {
        m_uiFactionTimer         = 8000;
        m_uiEnvelopingWindsTimer = 9000;
        m_uiShockTimer           = 5000;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        DoScriptText(SAY_SUMMON, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiFactionTimer)
        {
            if (m_uiFactionTimer <= uiDiff)
            {
                m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
                m_uiFactionTimer = 0;
            }
            else
                m_uiFactionTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 30.0f)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_creature->RemoveAllAuras();
            m_creature->DeleteThreatList();
            m_creature->CombatStop(true);
            DoScriptText(SAY_FREE, m_creature);
            return;
        }

        if (m_uiShockTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShockTimer = 10000;
        }
        else
            m_uiShockTimer -= uiDiff;

        if (m_uiEnvelopingWindsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENVELOPING_WINDS);
            m_uiEnvelopingWindsTimer = 25000;
        }
        else
            m_uiEnvelopingWindsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aeranas(Creature* pCreature)
{
    return new npc_aeranasAI(pCreature);
}

/*######
## npc_ancestral_wolf
######*/

enum
{
    EMOTE_WOLF_LIFT_HEAD            = -1000496,
    EMOTE_WOLF_HOWL                 = -1000497,
    SAY_WOLF_WELCOME                = -1000498,

    SPELL_ANCESTRAL_WOLF_BUFF       = 29981,

    NPC_RYGA                        = 17123
};

struct npc_ancestral_wolfAI : public npc_escortAI
{
    npc_ancestral_wolfAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
            Start(false, (Player*)pCreature->GetOwner());
        else
            script_error_log("npc_ancestral_wolf can not obtain owner or owner is not a player.");

        Reset();
    }

    void Reset() override
    {
        m_creature->CastSpell(m_creature, SPELL_ANCESTRAL_WOLF_BUFF, true);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(EMOTE_WOLF_LIFT_HEAD, m_creature);
                break;
            case 2:
                DoScriptText(EMOTE_WOLF_HOWL, m_creature);
                break;
            case 50:
                Creature* pRyga = GetClosestCreatureWithEntry(m_creature, NPC_RYGA, 30.0f);
                if (pRyga && pRyga->isAlive() && !pRyga->isInCombat())
                    DoScriptText(SAY_WOLF_WELCOME, pRyga);
                break;
        }
    }
};

CreatureAI* GetAI_npc_ancestral_wolf(Creature* pCreature)
{
    return new npc_ancestral_wolfAI(pCreature);
}

/*######
## npc_demoniac_scryer
######*/

#define GOSSIP_ITEM_ATTUNE          "Yes, Scryer. You may possess me."

enum
{
    GOSSIP_TEXTID_PROTECT           = 10659,
    GOSSIP_TEXTID_ATTUNED           = 10643,

    QUEST_DEMONIAC                  = 10838,
    NPC_HELLFIRE_WARDLING           = 22259,
    NPC_BUTTRESS                    = 22267,                // the 4x nodes
    NPC_SPAWNER                     = 22260,                // just a dummy, not used

    MAX_BUTTRESS                    = 4,
    TIME_TOTAL                      = MINUTE * 10 * IN_MILLISECONDS,

    SPELL_SUMMONED_DEMON            = 7741,                 // visual spawn-in for demon
    SPELL_DEMONIAC_VISITATION       = 38708,                // create item

    SPELL_BUTTRESS_APPERANCE        = 38719,                // visual on 4x bunnies + the flying ones
    SPELL_SUCKER_CHANNEL            = 38721,                // channel to the 4x nodes
    SPELL_SUCKER_DESPAWN_MOB        = 38691
};

// script is basic support, details like end event are not implemented
struct npc_demoniac_scryerAI : public ScriptedAI
{
    npc_demoniac_scryerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsComplete = false;
        m_uiSpawnDemonTimer = 15000;
        m_uiSpawnButtressTimer = 45000;
        m_uiButtressCount = 0;
        Reset();
    }

    bool m_bIsComplete;

    uint32 m_uiSpawnDemonTimer;
    uint32 m_uiSpawnButtressTimer;
    uint32 m_uiButtressCount;

    void Reset() override {}

    // we don't want anything to happen when attacked
    void AttackedBy(Unit* /*pEnemy*/) override {}
    void AttackStart(Unit* /*pEnemy*/) override {}

    void DoSpawnButtress()
    {
        ++m_uiButtressCount;

        float fAngle = 0.0f;

        switch (m_uiButtressCount)
        {
            case 1: fAngle = 0.0f; break;
            case 2: fAngle = M_PI_F + M_PI_F / 2; break;
            case 3: fAngle = M_PI_F / 2; break;
            case 4: fAngle = M_PI_F; break;
        }

        float fX, fY, fZ;
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0.0f, 5.0f, fAngle);

        uint32 uiTime = TIME_TOTAL - (m_uiSpawnButtressTimer * m_uiButtressCount);
        m_creature->SummonCreature(NPC_BUTTRESS, fX, fY, fZ, m_creature->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
    }

    void DoSpawnDemon()
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);

        m_creature->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED_DEMON, false);
            pSummoned->AI()->AttackStart(m_creature);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_BUTTRESS)
            {
                pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, false);
                pSummoned->CastSpell(m_creature, SPELL_SUCKER_CHANNEL, true);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetEntry() == NPC_HELLFIRE_WARDLING && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
            ((Creature*)pTarget)->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsComplete || !m_creature->isAlive())
            return;

        if (m_uiSpawnButtressTimer <= uiDiff)
        {
            if (m_uiButtressCount >= MAX_BUTTRESS)
            {
                m_creature->CastSpell(m_creature, SPELL_SUCKER_DESPAWN_MOB, false);

                if (m_creature->isInCombat())
                {
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop();
                }

                m_bIsComplete = true;
                return;
            }

            m_uiSpawnButtressTimer = 45000;
            DoSpawnButtress();
        }
        else
            m_uiSpawnButtressTimer -= uiDiff;

        if (m_uiSpawnDemonTimer <= uiDiff)
        {
            DoSpawnDemon();
            m_uiSpawnDemonTimer = 15000;
        }
        else
            m_uiSpawnDemonTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_demoniac_scryer(Creature* pCreature)
{
    return new npc_demoniac_scryerAI(pCreature);
}

bool GossipHello_npc_demoniac_scryer(Player* pPlayer, Creature* pCreature)
{
    if (npc_demoniac_scryerAI* pScryerAI = dynamic_cast<npc_demoniac_scryerAI*>(pCreature->AI()))
    {
        if (pScryerAI->m_bIsComplete)
        {
            if (pPlayer->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ATTUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ATTUNED, pCreature->GetObjectGuid());
            return true;
        }
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_PROTECT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_demoniac_scryer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, false);
    }

    return true;
}

/*######
## npc_wounded_blood_elf
######*/

enum
{
    SAY_ELF_START               = -1000117,
    SAY_ELF_SUMMON1             = -1000118,
    SAY_ELF_RESTING             = -1000119,
    SAY_ELF_SUMMON2             = -1000120,
    SAY_ELF_COMPLETE            = -1000121,
    SAY_ELF_AGGRO               = -1000122,

    NPC_WINDWALKER              = 16966,
    NPC_TALONGUARD              = 16967,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375,
};

struct npc_wounded_blood_elfAI : public npc_escortAI
{
    npc_wounded_blood_elfAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    void WaypointReached(uint32 uiPointId) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_ELF_START, m_creature, pPlayer);
                break;
            case 9:
                DoScriptText(SAY_ELF_SUMMON1, m_creature, pPlayer);
                // Spawn two Haal'eshi Talonguard
                DoSpawnCreature(NPC_WINDWALKER, -15, -15, 0, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                DoSpawnCreature(NPC_WINDWALKER, -17, -17, 0, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
            case 13:
                DoScriptText(SAY_ELF_RESTING, m_creature, pPlayer);
                break;
            case 14:
                DoScriptText(SAY_ELF_SUMMON2, m_creature, pPlayer);
                // Spawn two Haal'eshi Windwalker
                DoSpawnCreature(NPC_WINDWALKER, -15, -15, 0, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                DoSpawnCreature(NPC_WINDWALKER, -17, -17, 0, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                break;
            case 27:
                DoScriptText(SAY_ELF_COMPLETE, m_creature, pPlayer);
                // Award quest credit
                pPlayer->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH, m_creature);
                break;
        }
    }

    void Reset() override { }

    void Aggro(Unit* /*pWho*/) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            DoScriptText(SAY_ELF_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }
};

CreatureAI* GetAI_npc_wounded_blood_elf(Creature* pCreature)
{
    return new npc_wounded_blood_elfAI(pCreature);
}

bool QuestAccept_npc_wounded_blood_elf(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
    {
        // Change faction so mobs attack
        pCreature->SetFactionTemporary(FACTION_ESCORT_H_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        if (npc_wounded_blood_elfAI* pEscortAI = dynamic_cast<npc_wounded_blood_elfAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }

    return true;
}

/*######
## npc_fel_guard_hound
######*/

enum
{
    SPELL_CREATE_POODAD         = 37688,
    SPELL_FAKE_DOG_SPART        = 37692,
    SPELL_INFORM_DOG            = 37689,

    NPC_DERANGED_HELBOAR        = 16863,
};

struct npc_fel_guard_houndAI : public ScriptedPetAI
{
    npc_fel_guard_houndAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    uint32 m_uiPoodadTimer;

    bool m_bIsPooActive;

    void Reset() override
    {
        m_uiPoodadTimer = 0;
        m_bIsPooActive  = false;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (DoCastSpellIfCan(m_creature, SPELL_FAKE_DOG_SPART) == CAST_OK)
            m_uiPoodadTimer = 2000;
    }

    // Function to allow the boar to move to target
    void DoMoveToCorpse(Unit* pBoar)
    {
        if (!pBoar)
            return;

        m_bIsPooActive = true;
        m_creature->GetMotionMaster()->MovePoint(1, pBoar->GetPositionX(), pBoar->GetPositionY(), pBoar->GetPositionZ());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiPoodadTimer)
        {
            if (m_uiPoodadTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CREATE_POODAD) == CAST_OK)
                {
                    m_uiPoodadTimer = 0;
                    m_bIsPooActive = false;
                }
            }
            else
                m_uiPoodadTimer -= uiDiff;
        }

        if (!m_bIsPooActive)
            ScriptedPetAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_npc_fel_guard_hound(Creature* pCreature)
{
    return new npc_fel_guard_houndAI(pCreature);
}

bool EffectDummyCreature_npc_fel_guard_hound(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_INFORM_DOG && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetEntry() == NPC_DERANGED_HELBOAR)
        {
            if (npc_fel_guard_houndAI* pHoundAI = dynamic_cast<npc_fel_guard_houndAI*>(pCreatureTarget->AI()))
                pHoundAI->DoMoveToCorpse(pCaster);
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_anchorite_barada
######*/

enum
{
    SAY_EXORCISM_1                  = -1000981,
    SAY_EXORCISM_2                  = -1000982,
    SAY_EXORCISM_3                  = -1000983,
    SAY_EXORCISM_4                  = -1000984,
    SAY_EXORCISM_5                  = -1000985,
    SAY_EXORCISM_6                  = -1000986,

    SPELL_BARADA_COMMANDS           = 39277,
    SPELL_BARADA_FALTERS            = 39278,

    SPELL_JULES_THREATENS           = 39284,
    SPELL_JULES_GOES_UPRIGHT        = 39294,
    SPELL_JULES_VOMITS              = 39295,
    SPELL_JULES_RELEASE_DARKNESS    = 39306,                // periodic trigger missing spell 39305

    NPC_ANCHORITE_BARADA            = 22431,
    NPC_COLONEL_JULES               = 22432,
    NPC_DARKNESS_RELEASED           = 22507,                // summoned by missing spell 39305

    GOSSIP_ITEM_EXORCISM            = -3000111,
    QUEST_ID_EXORCISM               = 10935,

    TEXT_ID_CLEANSED                = 10706,
    TEXT_ID_POSSESSED               = 10707,
    TEXT_ID_ANCHORITE               = 10683,
};

static const DialogueEntry aExorcismDialogue[] =
{
    {SAY_EXORCISM_1,        NPC_ANCHORITE_BARADA,   3000},
    {SAY_EXORCISM_2,        NPC_ANCHORITE_BARADA,   2000},
    {QUEST_ID_EXORCISM,     0,                      0},         // start wp movemnet
    {SAY_EXORCISM_3,        NPC_COLONEL_JULES,      3000},
    {SPELL_BARADA_COMMANDS, 0,                      10000},
    {SAY_EXORCISM_4,        NPC_ANCHORITE_BARADA,   10000},
    {SAY_EXORCISM_5,        NPC_COLONEL_JULES,      10000},
    {SPELL_BARADA_FALTERS,  0,                      2000},
    {SPELL_JULES_THREATENS, 0,                      15000},     // start levitating
    {NPC_COLONEL_JULES,     0,                      15000},
    {NPC_ANCHORITE_BARADA,  0,                      15000},
    {NPC_COLONEL_JULES,     0,                      15000},
    {NPC_ANCHORITE_BARADA,  0,                      15000},
    {SPELL_JULES_GOES_UPRIGHT, 0,                   3000},
    {SPELL_JULES_VOMITS,    0,                      7000},      // start moving around the room
    {NPC_COLONEL_JULES,     0,                      10000},
    {NPC_ANCHORITE_BARADA,  0,                      10000},
    {NPC_COLONEL_JULES,     0,                      10000},
    {NPC_ANCHORITE_BARADA,  0,                      10000},
    {NPC_COLONEL_JULES,     0,                      10000},
    {NPC_ANCHORITE_BARADA,  0,                      10000},
    {NPC_COLONEL_JULES,     0,                      10000},
    {NPC_ANCHORITE_BARADA,  0,                      10000},
    {NPC_DARKNESS_RELEASED, 0,                      5000},      // event finished
    {SAY_EXORCISM_6,        NPC_ANCHORITE_BARADA,   3000},
    {TEXT_ID_CLEANSED,      0,                      0},
    {0, 0, 0},
};

static const int32 aAnchoriteTexts[3] = { -1000987, -1000988, -1000989 };
static const int32 aColonelTexts[3] = { -1000990, -1000991, -1000992 };

// Note: script is highly dependent on DBscript implementation
struct npc_anchorite_baradaAI : public ScriptedAI, private DialogueHelper
{
    npc_anchorite_baradaAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aExorcismDialogue)
    {
        Reset();
    }

    bool m_bEventComplete;
    bool m_bEventInProgress;

    ObjectGuid m_colonelGuid;

    void Reset() override
    {
        m_bEventComplete = false;
        m_bEventInProgress = false;
    }

    void AttackStart(Unit* pWho) override
    {
        // no attack during the exorcism
        if (m_bEventInProgress)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        // no evade during the exorcism
        if (m_bEventInProgress)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    bool IsExorcismComplete() { return m_bEventComplete; }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            // start the actuall exorcism
            if (Creature* pColoner = GetClosestCreatureWithEntry(m_creature, NPC_COLONEL_JULES, 15.0f))
                m_colonelGuid = pColoner->GetObjectGuid();

            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            StartNextDialogueText(SAY_EXORCISM_1);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case 3:
                // pause wp and resume dialogue
                m_creature->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_bEventInProgress = true;

                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    m_creature->SetFacingToObject(pColonel);
                    pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }

                StartNextDialogueText(SAY_EXORCISM_3);
                break;
            case 6:
                // event completed - wait for player to get quest credit by gossip
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    m_creature->SetFacingToObject(pColonel);
                m_creature->GetMotionMaster()->Clear();
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_bEventComplete = true;
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case QUEST_ID_EXORCISM:
                m_creature->GetMotionMaster()->MoveWaypoint();
                break;
            case SPELL_BARADA_COMMANDS:
                DoCastSpellIfCan(m_creature, SPELL_BARADA_COMMANDS);
                break;
            case SPELL_BARADA_FALTERS:
                DoCastSpellIfCan(m_creature, SPELL_BARADA_FALTERS);
                // start levitating
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->SetLevitate(true);
                    pColonel->GetMotionMaster()->MovePoint(0, pColonel->GetPositionX(), pColonel->GetPositionY(), pColonel->GetPositionZ() + 2.0f);
                }
                break;
            case SPELL_JULES_THREATENS:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->CastSpell(pColonel, SPELL_JULES_THREATENS, true);
                    pColonel->CastSpell(pColonel, SPELL_JULES_RELEASE_DARKNESS, true);
                    pColonel->SetFacingTo(0);
                }
                break;
            case SPELL_JULES_GOES_UPRIGHT:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->InterruptNonMeleeSpells(false);
                    pColonel->CastSpell(pColonel, SPELL_JULES_GOES_UPRIGHT, false);
                }
                break;
            case SPELL_JULES_VOMITS:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->CastSpell(pColonel, SPELL_JULES_VOMITS, true);
                    pColonel->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 3.0f, 5.0f);
                }
                break;
            case NPC_COLONEL_JULES:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    DoScriptText(aColonelTexts[urand(0, 2)], pColonel);
                break;
            case NPC_ANCHORITE_BARADA:
                DoScriptText(aAnchoriteTexts[urand(0, 2)], m_creature);
                break;
            case NPC_DARKNESS_RELEASED:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->RemoveAurasDueToSpell(SPELL_JULES_THREATENS);
                    pColonel->RemoveAurasDueToSpell(SPELL_JULES_RELEASE_DARKNESS);
                    pColonel->RemoveAurasDueToSpell(SPELL_JULES_VOMITS);
                    pColonel->GetMotionMaster()->MoveTargetedHome();
                    pColonel->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                break;
            case TEXT_ID_CLEANSED:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->RemoveAurasDueToSpell(SPELL_JULES_GOES_UPRIGHT);
                    pColonel->SetLevitate(false);
                }
                // resume wp movemnet
                m_creature->RemoveAllAuras();
                m_creature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
            case NPC_ANCHORITE_BARADA:      return m_creature;
            case NPC_COLONEL_JULES:         return m_creature->GetMap()->GetCreature(m_colonelGuid);

            default:
                return NULL;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_anchorite_barada(Creature* pCreature)
{
    return new npc_anchorite_baradaAI(pCreature);
}

bool GossipHello_npc_anchorite_barada(Player* pPlayer, Creature* pCreature)
{
    // check if quest is active but not completed
    if (pPlayer->IsCurrentQuest(QUEST_ID_EXORCISM, 1))
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_EXORCISM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ANCHORITE, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_anchorite_barada(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pCreature);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_colonel_jules
######*/

bool GossipHello_npc_colonel_jules(Player* pPlayer, Creature* pCreature)
{
    // quest already completed
    if (pPlayer->GetQuestStatus(QUEST_ID_EXORCISM) == QUEST_STATUS_COMPLETE)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_CLEANSED, pCreature->GetObjectGuid());
        return true;
    }
    // quest active but not complete
    else if (pPlayer->IsCurrentQuest(QUEST_ID_EXORCISM, 1))
    {
        Creature* pAnchorite = GetClosestCreatureWithEntry(pCreature, NPC_ANCHORITE_BARADA, 15.0f);
        if (!pAnchorite)
            return true;

        if (npc_anchorite_baradaAI* pAnchoriteAI = dynamic_cast<npc_anchorite_baradaAI*>(pAnchorite->AI()))
        {
            // event complete - give credit and reset
            if (pAnchoriteAI->IsExorcismComplete())
            {
                // kill credit
                pPlayer->RewardPlayerAndGroupAtEvent(pCreature->GetEntry(), pCreature);

                // reset Anchorite and Colonel
                pAnchorite->AI()->EnterEvadeMode();
                pCreature->AI()->EnterEvadeMode();

                pAnchorite->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_CLEANSED, pCreature->GetObjectGuid());
                return true;
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_POSSESSED, pCreature->GetObjectGuid());
    return true;
}

bool EffectDummyCreature_npc_colonel_jules(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_JULES_RELEASE_DARKNESS && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_COLONEL_JULES)
    {
        Creature* pAnchorite = GetClosestCreatureWithEntry(pCreatureTarget, NPC_ANCHORITE_BARADA, 15.0f);
        if (!pAnchorite)
            return false;

        // get random point around the Anchorite
        float fX, fY, fZ;
        pCreatureTarget->GetNearPoint(pCreatureTarget, fX, fY, fZ, 5.0f, 10.0f, frand(0, M_PI_F / 2));

        // spawn a Darkness Released npc and move around the room
        if (Creature* pDarkness = pCreatureTarget->SummonCreature(NPC_DARKNESS_RELEASED, 0, 0, 0, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 20000))
            pDarkness->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_magister_aledis
######*/

enum
{
    SAY_ALEDIS_DEFEAT               = -1001172,

    SPELL_PYROBLAST                 = 33975,
    SPELL_FROST_NOVA                = 11831,
    SPELL_FIREBALL                  = 20823,
};

struct npc_magister_aledisAI : public ScriptedAI
{
    npc_magister_aledisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsDefeated = false;
        Reset();
    }

    uint32 m_uiPyroblastTimer;
    uint32 m_uiFrostNovaTimer;
    uint32 m_uiFireballTimer;

    bool m_bIsDefeated;

    void Reset() override
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        m_uiPyroblastTimer      = urand(10000, 14000);
        m_uiFrostNovaTimer      = 0;
        m_uiFireballTimer       = 1000;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, false))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 10.0f);
        }
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        if (!m_bIsDefeated)
            m_creature->LoadCreatureAddon(true);

        if (m_creature->isAlive())
        {
            if (!m_bIsDefeated)
            {
                m_creature->SetWalk(true);
                m_creature->GetMotionMaster()->MoveWaypoint();
            }
            else
                m_creature->GetMotionMaster()->MoveIdle();
        }

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsDefeated && m_creature->GetHealthPercent() < 25.0f)
        {
            // evade when defeated; faction is reset automatically
            m_bIsDefeated = true;
            EnterEvadeMode();

            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            DoScriptText(SAY_ALEDIS_DEFEAT, m_creature);
            m_creature->ForcedDespawn(60000);
            return;
        }

        if (m_uiPyroblastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PYROBLAST) == CAST_OK)
                m_uiPyroblastTimer = urand(18000, 21000);
        }
        else
            m_uiPyroblastTimer -= uiDiff;

        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireballTimer = urand(3000, 4000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiFrostNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FROST_NOVA) == CAST_OK)
                m_uiFrostNovaTimer = urand(12000, 16000);
        }
        else
            m_uiFrostNovaTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_magister_aledis(Creature* pCreature)
{
    return new npc_magister_aledisAI(pCreature);
}

void AddSC_hellfire_peninsula()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_aeranas";
    pNewScript->GetAI = &GetAI_npc_aeranas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ancestral_wolf";
    pNewScript->GetAI = &GetAI_npc_ancestral_wolf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_demoniac_scryer";
    pNewScript->GetAI = &GetAI_npc_demoniac_scryer;
    pNewScript->pGossipHello = &GossipHello_npc_demoniac_scryer;
    pNewScript->pGossipSelect = &GossipSelect_npc_demoniac_scryer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wounded_blood_elf";
    pNewScript->GetAI = &GetAI_npc_wounded_blood_elf;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_wounded_blood_elf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fel_guard_hound";
    pNewScript->GetAI = &GetAI_npc_fel_guard_hound;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_fel_guard_hound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_anchorite_barada";
    pNewScript->GetAI = &GetAI_npc_anchorite_barada;
    pNewScript->pGossipHello = &GossipHello_npc_anchorite_barada;
    pNewScript->pGossipSelect = &GossipSelect_npc_anchorite_barada;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_colonel_jules";
    pNewScript->pGossipHello = &GossipHello_npc_colonel_jules;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_colonel_jules;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_magister_aledis";
    pNewScript->GetAI = &GetAI_npc_magister_aledis;
    pNewScript->RegisterSelf();
}
