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
SDComment: Quest support: 9375, 9410, 9418, 10286, 10629, 10838, 10935, 11516.
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
npc_living_flare
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"

/*######
## npc_aeranas
######*/

enum
{
    SAY_FREE                        = -1000139,

    FACTION_HOSTILE                 = 16,
    FACTION_FRIENDLY                = 35,

    SPELL_ENVELOPING_WINDS          = 15535,
    SPELL_SHOCK                     = 12553,
};

struct npc_aeranasAI : public ScriptedAI
{
    npc_aeranasAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiEnvelopingWindsTimer;
    uint32 m_uiShockTimer;

    void Reset() override
    {
        m_uiEnvelopingWindsTimer = 9000;
        m_uiShockTimer           = 5000;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff) override
    {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 30.0f)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_creature->RemoveAllAuras();
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

UnitAI* GetAI_npc_aeranas(Creature* pCreature)
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
        DoCastSpellIfCan(m_creature, SPELL_ANCESTRAL_WOLF_BUFF);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateEscortAI(const uint32 uiDiff) override { };

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

UnitAI* GetAI_npc_ancestral_wolf(Creature* pCreature)
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
        m_creature->SummonCreature(NPC_BUTTRESS, fX, fY, fZ, m_creature->GetAngle(fX, fY), TEMPSPAWN_TIMED_DESPAWN, uiTime);
    }

    void DoSpawnDemon()
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);

        m_creature->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 5000);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED_DEMON, TRIGGERED_NONE);
            pSummoned->AI()->AttackStart(m_creature);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_BUTTRESS)
            {
                pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, TRIGGERED_NONE);
                pSummoned->CastSpell(m_creature, SPELL_SUCKER_CHANNEL, TRIGGERED_OLD_TRIGGERED);
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
                m_creature->CastSpell(m_creature, SPELL_SUCKER_DESPAWN_MOB, TRIGGERED_NONE);

                if (m_creature->isInCombat())
                {
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

UnitAI* GetAI_npc_demoniac_scryer(Creature* pCreature)
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
        pCreature->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, TRIGGERED_NONE);
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
    SAY_ELF_JUST_KILLED         = -1000122,

    NPC_WINDWALKER              = 16966,
    NPC_TALONGUARD              = 16967,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375,

    DBSCRIPT_END_TALERIS_INT	= 10028,

    FACTION_TROLL_FROSTMANE = 33
};

struct npc_wounded_blood_elfAI : public npc_escortAI
{
    npc_wounded_blood_elfAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    void Reset() override { }

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
            case 13:
                m_creature->SetFacingTo(3.7f);
                DoScriptText(SAY_ELF_SUMMON1, m_creature, pPlayer);
                m_creature->SummonCreature(NPC_TALONGUARD, -983.206f, 4163.884f, 38.01043f, 0.72f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_TALONGUARD, -985.732f, 4157.546f, 43.50933f, 0.80f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                break;
            case 19:
                DoScriptText(SAY_ELF_RESTING, m_creature, pPlayer);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 20:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case 21:
                DoScriptText(SAY_ELF_SUMMON2, m_creature, pPlayer);
                m_creature->SummonCreature(NPC_WINDWALKER, -864.001f, 4253.191f, 43.89390f, 5.27f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                break;
            case 38:
                DoScriptText(SAY_ELF_COMPLETE, m_creature, pPlayer);
                break;
            case 39:
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                break;
            case 40:
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ROAD_TO_FALCON_WATCH, m_creature);
                pPlayer->GetMap()->ScriptsStart(sRelayScripts, DBSCRIPT_END_TALERIS_INT, m_creature, m_creature);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AddThreat(m_creature);
        pSummoned->SetInCombatWith(m_creature);
        m_creature->SetInCombatWith(pSummoned);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 3))
            DoScriptText(SAY_ELF_JUST_KILLED, m_creature);
    }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }
};

UnitAI* GetAI_npc_wounded_blood_elf(Creature* pCreature)
{
    return new npc_wounded_blood_elfAI(pCreature);
}

bool QuestAccept_npc_wounded_blood_elf(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
    {
        // Change faction so mobs attack
        pCreature->SetFactionTemporary(FACTION_TROLL_FROSTMANE, TEMPFACTION_RESTORE_RESPAWN);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);

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

UnitAI* GetAI_npc_fel_guard_hound(Creature* pCreature)
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
    SAY_EXORCISM_4                  = -1000991,
    SAY_EXORCISM_5					= -1000988,
    SAY_EXORCISM_6					= -1000992,
    SAY_EXORCISM_7					= -1000986,
    
    BARADA_SAY_EXORCISM_RANDOM_1	= -1000987,
    BARADA_SAY_EXORCISM_RANDOM_2	= -1015009,
    BARADA_SAY_EXORCISM_RANDOM_3	= -1000989,
    BARADA_SAY_EXORCISM_RANDOM_4	= -1015010,
    BARADA_SAY_EXORCISM_RANDOM_5	= -1000984,
    BARADA_SAY_EXORCISM_RANDOM_6	= -1015005,
    BARADA_SAY_EXORCISM_RANDOM_7	= -1015007,

    JULES_SAY_EXORCISM_RANDOM_1		= -1000990,
    JULES_SAY_EXORCISM_RANDOM_2		= -1015006,
    JULES_SAY_EXORCISM_RANDOM_3		= -1015008,
    JULES_SAY_EXORCISM_RANDOM_4		= -1015011,
    JULES_SAY_EXORCISM_RANDOM_5	    = -1000985,

    SPELL_BARADA_COMMANDS           = 39277,
    SPELL_BARADA_FALTERS            = 39278,
    SPELL_HEAL_SELF					= 39321,

    SPELL_JULES_THREATENS_AURA	    = 39284,
    SPELL_JULES_GOES_UPRIGHT        = 39294,
    SPELL_JULES_VOMITS              = 39295,
    SPELL_JULES_RELEASE_DARKNESS    = 39306,                // periodic trigger spell 39305
    SPELL_JULES_GOES_PRONE          = 39283,
    SPELL_FLYING_SKULL_DESPAWN      = 39307,

    NPC_ANCHORITE_BARADA            = 22431,
    NPC_COLONEL_JULES               = 22432,
    NPC_DARKNESS_RELEASED           = 22507,                // summoned by spell 39305
    NPC_FOUL_PURGE                  = 22506,
    NPC_BUBBLING_SLIMER_BUNNY       = 22505,
    NPC_EXORCISM_LIGHTNING_CLOUD    = 22508,

    GOSSIP_ITEM_EXORCISM            = -3000111,
    QUEST_ID_EXORCISM               = 10935,

    TEXT_ID_CLEANSED                = 10706,
    TEXT_ID_POSSESSED               = 10707,
    TEXT_ID_ANCHORITE               = 10683,
};


/*######
## npc_colonel_jules
######*/

struct npc_colonel_julesAI : public ScriptedAI
{
    npc_colonel_julesAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bReturnHome;
    std::vector<ObjectGuid> m_vSummonVector;

    void Reset() override
    {
        for (ObjectGuid& guid : m_vSummonVector)
            if (Creature* summon = m_creature->GetMap()->GetCreature(guid))
                summon->ForcedDespawn();

        m_vSummonVector.clear();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE)
        {
            if (uiType == POINT_MOTION_TYPE)
                if (uiPointId == 10)
                    m_creature->SetFacingTo(3.1f);

            return;
        }

        if (m_bReturnHome)
        {
            if (uiPointId == 6)
            {
                m_creature->SetLevitate(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->SetFacingTo(1.46213f);
                m_creature->RemoveAurasDueToSpell(SPELL_JULES_GOES_UPRIGHT);
                m_creature->CastSpell(m_creature, SPELL_JULES_GOES_PRONE, TRIGGERED_OLD_TRIGGERED);

                EndEvent();
            }
        }
        else if (uiPointId == 4)
            m_creature->GetMotionMaster()->SetNextWaypoint(0);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_vSummonVector.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DARKNESS_RELEASED)
            pSummoned->CastSpell(pSummoned, SPELL_FLYING_SKULL_DESPAWN, TRIGGERED_OLD_TRIGGERED);

        m_vSummonVector.erase(std::remove(m_vSummonVector.begin(), m_vSummonVector.end(), pSummoned->GetObjectGuid()), m_vSummonVector.end());
    }

    void GoHome()
    {
        m_bReturnHome = true;
        m_creature->GetMotionMaster()->SetNextWaypoint(5);
        m_creature->RemoveAurasDueToSpell(SPELL_JULES_VOMITS);
        m_creature->RemoveAurasDueToSpell(SPELL_JULES_THREATENS_AURA);
        m_creature->RemoveAurasDueToSpell(SPELL_JULES_RELEASE_DARKNESS);
    }

    void EndEvent()
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        Reset();
    }
};

UnitAI* GetAI_npc_colonel_jules(Creature* pCreature)
{
    return new npc_colonel_julesAI(pCreature);
}

static const DialogueEntry aExorcismDialogue[] =
{
    {SAY_EXORCISM_1,            NPC_ANCHORITE_BARADA, 2500},
    {SAY_EXORCISM_2,            NPC_ANCHORITE_BARADA, 3000},
    {QUEST_ID_EXORCISM,         0,                    2000},	// start wp movemnet
    {SAY_EXORCISM_3,            NPC_COLONEL_JULES,    20000},
    {TEXT_ID_POSSESSED,			0,					  1500},
    {SPELL_FLYING_SKULL_DESPAWN,0,					  2500},	// kneel
    {SPELL_BARADA_COMMANDS,     0,					  8000},
    {NPC_EXORCISM_LIGHTNING_CLOUD, 0,				  2000},	// summon Lightning Bunny
    {NPC_ANCHORITE_BARADA,      0,					  10000},	// random
    {NPC_COLONEL_JULES,         0,					  10000},	// r
    {SPELL_BARADA_FALTERS,      0,					  2000},	// start levitating
    {SPELL_JULES_THREATENS_AURA,0,					  8000},
    {NPC_ANCHORITE_BARADA,      0,		              10000},	// r
    {NPC_ANCHORITE_BARADA,      0,		              10000},	// r
    {NPC_ANCHORITE_BARADA,      0,		              14000},	// r
    {NPC_ANCHORITE_BARADA,      0,                    24000},	// r
    {NPC_ANCHORITE_BARADA,      0,					  3000},	// r
    {SPELL_BARADA_COMMANDS,     0,					  4000},
    {SPELL_JULES_GOES_UPRIGHT,  0,					  3000},
    {SPELL_JULES_VOMITS,        0,					  6000},    // start moving around the room
    {NPC_COLONEL_JULES,         0,                    10000},	// r
    {NPC_ANCHORITE_BARADA,      0,                    10000},	// r
    {NPC_COLONEL_JULES,         0,                    10000},	// r
    {NPC_ANCHORITE_BARADA,      0,                    10000},	// r
    {SAY_EXORCISM_4,		    NPC_COLONEL_JULES,    10000},
    {SAY_EXORCISM_5,		    NPC_ANCHORITE_BARADA, 10000},
    {SAY_EXORCISM_6,		    NPC_COLONEL_JULES,	  10000},
    {NPC_ANCHORITE_BARADA,      0,                    10000},
    {NPC_BUBBLING_SLIMER_BUNNY, 0,					  5000},	// jules moves back 
    {SAY_EXORCISM_7,            NPC_ANCHORITE_BARADA, 3000},	// bara stands, bows
    {NPC_FOUL_PURGE,			0,					  11500},	// bara walks back
    {0, 0, 0}
};

static const int32 aAnchoriteTexts[7] = {BARADA_SAY_EXORCISM_RANDOM_1, BARADA_SAY_EXORCISM_RANDOM_2, BARADA_SAY_EXORCISM_RANDOM_3, BARADA_SAY_EXORCISM_RANDOM_4, BARADA_SAY_EXORCISM_RANDOM_5, BARADA_SAY_EXORCISM_RANDOM_6, BARADA_SAY_EXORCISM_RANDOM_7};
static const int32 aColonelTexts[5] = {JULES_SAY_EXORCISM_RANDOM_1, JULES_SAY_EXORCISM_RANDOM_2, JULES_SAY_EXORCISM_RANDOM_3, JULES_SAY_EXORCISM_RANDOM_4, JULES_SAY_EXORCISM_RANDOM_5};

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

    uint32 m_uiResetTimer;

    ObjectGuid m_colonelGuid;
    ObjectGuid m_playerGuid;
 
    void Reset() override
    {
        m_bEventComplete = false;
        m_bEventInProgress = false;
        m_uiResetTimer = 0;
        m_playerGuid.Clear();
        m_colonelGuid.Clear();
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

    bool IsExorcismComplete() const { return m_bEventComplete; }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (!m_bEventInProgress && eventType == AI_EVENT_START_EVENT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_bEventInProgress = true;

            m_playerGuid = pInvoker->GetObjectGuid();

            // start the actual exorcism
            if (Creature* pColonel = GetClosestCreatureWithEntry(m_creature, NPC_COLONEL_JULES, 15.0f))
            {
                m_colonelGuid = pColonel->GetObjectGuid();

                if (npc_colonel_julesAI* julesAI = dynamic_cast<npc_colonel_julesAI*>(pColonel->AI()))
                    julesAI->m_bReturnHome = false;
            }

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
            case 1:
                // pause wp and resume dialogue
                m_creature->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);

                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    m_creature->SetFacingToObject(pColonel);
                    pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }

                StartNextDialogueText(TEXT_ID_POSSESSED);
                break;
            case 3:
                // event completed - wait for player to get quest credit by gossip
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    m_creature->SetFacingToObject(pColonel);

                m_creature->CastSpell(m_creature, SPELL_HEAL_SELF, TRIGGERED_OLD_TRIGGERED);
                m_creature->GetMotionMaster()->Clear();
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_bEventComplete = true;
                m_uiResetTimer = 30000;
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SPELL_FLYING_SKULL_DESPAWN:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case QUEST_ID_EXORCISM:
                m_creature->GetMotionMaster()->MoveWaypoint();
                break;
            case SPELL_BARADA_COMMANDS:
                DoCastSpellIfCan(m_creature, SPELL_BARADA_COMMANDS);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case NPC_EXORCISM_LIGHTNING_CLOUD:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    pColonel->SummonCreature(NPC_EXORCISM_LIGHTNING_CLOUD, -709.8715f, 2756.557f, 113.6571f, 4.747295f, TEMPSPAWN_TIMED_DESPAWN, 200000);
                break;
            case SPELL_BARADA_FALTERS:
                DoCastSpellIfCan(m_creature, SPELL_BARADA_FALTERS);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                // start levitating
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->SetLevitate(true);
                    pColonel->GetMotionMaster()->MovePoint(10, pColonel->GetPositionX(), pColonel->GetPositionY(), pColonel->GetPositionZ() + 2.0f);
                }
                break;
            case SPELL_JULES_THREATENS_AURA:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    pColonel->CastSpell(pColonel, SPELL_JULES_THREATENS_AURA, TRIGGERED_OLD_TRIGGERED);
                break;
            case SPELL_JULES_GOES_UPRIGHT:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->InterruptNonMeleeSpells(false);
                    pColonel->RemoveAurasDueToSpell(SPELL_JULES_GOES_PRONE);
                    pColonel->CastSpell(pColonel, SPELL_JULES_GOES_UPRIGHT, TRIGGERED_NONE);
                    pColonel->CastSpell(pColonel, SPELL_JULES_RELEASE_DARKNESS, TRIGGERED_OLD_TRIGGERED);
                }
                break;
            case SPELL_JULES_VOMITS:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                {
                    pColonel->CastSpell(pColonel, SPELL_JULES_VOMITS, TRIGGERED_OLD_TRIGGERED);
                    // begin jules movement
                    pColonel->GetMotionMaster()->MoveWaypoint();
                }
                break;
            case NPC_ANCHORITE_BARADA:
                DoScriptText(aAnchoriteTexts[urand(0, 6)], m_creature);
                break;
            case NPC_COLONEL_JULES:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    DoScriptText(aColonelTexts[urand(0, 4)], pColonel);
                break;
            case NPC_BUBBLING_SLIMER_BUNNY:
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    if (npc_colonel_julesAI* julesAI = dynamic_cast<npc_colonel_julesAI*>(pColonel->AI()))
                        julesAI->GoHome();
                break;
            case SAY_EXORCISM_7:
                m_creature->RemoveAurasDueToSpell(SPELL_BARADA_COMMANDS);
                m_creature->RemoveAurasDueToSpell(SPELL_BARADA_FALTERS);
                break;
            case NPC_FOUL_PURGE:
                // resume wp movemnet
                m_creature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
        }
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_bEventInProgress)
        {
            if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                if (npc_colonel_julesAI* julesAI = dynamic_cast<npc_colonel_julesAI*>(pColonel->AI()))
                    julesAI->EndEvent();

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                pPlayer->FailQuest(QUEST_ID_EXORCISM);
        }
        else
            if (Creature* pColonel = GetClosestCreatureWithEntry(m_creature, NPC_COLONEL_JULES, 30.0f))
                pColonel->GetMotionMaster()->MovePoint(1, -710.038f, 2750.877f, 103.85f);
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
            case NPC_ANCHORITE_BARADA:      return m_creature;
            case NPC_COLONEL_JULES:         return m_creature->GetMap()->GetCreature(m_colonelGuid);

            default:
                return nullptr;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_playerGuid && m_bEventInProgress)
        {
            bool m_reset = false;
            if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                m_reset = !player->IsActiveQuest(QUEST_ID_EXORCISM);
            else 
                m_reset = true;

            if (m_reset)
            {
                if (Creature* pColonel = m_creature->GetMap()->GetCreature(m_colonelGuid))
                    if (npc_colonel_julesAI* julesAI = dynamic_cast<npc_colonel_julesAI*>(pColonel->AI()))
                        julesAI->EndEvent();

                if (m_creature->isAlive())
                    m_creature->ForcedDespawn();
                m_creature->Respawn();

                Reset();
                return;
            }
        }

        if (m_bEventInProgress)
            DialogueUpdate(uiDiff);

        if (m_uiResetTimer) // Remove finishability after some time
        {
            if (m_uiResetTimer <= uiDiff)
            {
                //Reset(); just calling reset was messing up his waypointing...
                m_creature->ForcedDespawn();
                m_creature->Respawn();
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_anchorite_barada(Creature* pCreature)
{
    return new npc_anchorite_baradaAI(pCreature);
}

bool GossipHello_npc_anchorite_barada(Player* pPlayer, Creature* pCreature)
{
    // check if quest is active but not completed
    if (pPlayer->IsCurrentQuest(QUEST_ID_EXORCISM, 1))
        if (npc_anchorite_baradaAI* baradaAI = dynamic_cast<npc_anchorite_baradaAI*>(pCreature->AI()))
            if (!baradaAI->m_bEventInProgress)
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

bool GossipHello_npc_colonel_jules(Player* pPlayer, Creature* pCreature)
{
    // quest already completed
    if (pPlayer->GetQuestStatus(QUEST_ID_EXORCISM) == QUEST_STATUS_COMPLETE)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_CLEANSED, pCreature->GetObjectGuid());
        return true;
    }
    // quest active but not complete
    if (pPlayer->IsCurrentQuest(QUEST_ID_EXORCISM, 1))
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
                pPlayer->RewardPlayerAndGroupAtEventCredit(pCreature->GetEntry(), pCreature);

                pAnchorite->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_CLEANSED, pCreature->GetObjectGuid());
                return true;
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_POSSESSED, pCreature->GetObjectGuid());
    return true;
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

        m_attackDistance = 10.0f;
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
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

        m_creature->SetLootRecipient(nullptr);

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

UnitAI* GetAI_npc_magister_aledis(Creature* pCreature)
{
    return new npc_magister_aledisAI(pCreature);
}

/*######
## npc_living_flare
######*/

enum
{
    EMOTE_FLARE_UNSTABLE            = -1001214,
    EMOTE_FLARE_BURST               = -1001215,

    SPELL_LIVING_COSMETIC           = 44880,                // cosmetic spell
    SPELL_LIVING_FLARE_MASTER       = 44877,                // dummy aura used to hit the pet
    SPELL_FEL_FLAREUP               = 44944,                // scale up pet
    SPELL_LIVING_FLARE_UNSTABLE     = 44943,                // visual to transform
    SPELL_UNSTABLE_COSMETIC         = 46196,                // cosmetic spell
    //SPELL_LIVING_FLARE_DETONATOR  = 44948,                // possible used check for the generic quest trigger; needs more research
    SPELL_COSMETIC_EXPLOSION        = 46225,                // ToDo: confirm spell
    SPELL_QUEST_CREDIT              = 44947,                // quest complete spell

    NPC_UNSTABLE_LIVING_FLARE       = 24958,
    NPC_GENERIC_QUEST_TRIGGER       = 24959,                // dummy npc used to check for the gate
    GO_LARGE_FIRE                   = 187084,

    MAX_FLAREUP_STACKS              = 8,
};

struct npc_living_flareAI : public ScriptedPetAI
{
    npc_living_flareAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    bool m_bCheckComplete;
    uint32 m_uiCheckTimer;

    void Reset() override
    {
        m_uiCheckTimer      = 0;
        m_bCheckComplete    = false;

        DoCastSpellIfCan(m_creature, SPELL_LIVING_COSMETIC);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(EMOTE_FLARE_UNSTABLE, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_LIVING_COSMETIC);
            // Note: on updateEntry the scale of the object should be persistent; requires core fix
            m_creature->UpdateEntry(NPC_UNSTABLE_LIVING_FLARE);
            DoCastSpellIfCan(m_creature, SPELL_LIVING_FLARE_UNSTABLE, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_COSMETIC, CAST_TRIGGERED);
            m_uiCheckTimer = 1000;
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (!uiPointId)
            return;

        DoCastSpellIfCan(m_creature, SPELL_QUEST_CREDIT, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_COSMETIC_EXPLOSION, CAST_TRIGGERED);
        DoScriptText(EMOTE_FLARE_BURST, m_creature);
        m_creature->ForcedDespawn(2000);

        // respawn all fires in range
        std::list<GameObject*> lFiresInRange;
        GetGameObjectListWithEntryInGrid(lFiresInRange, m_creature, GO_LARGE_FIRE, 50.0f);

        if (lFiresInRange.empty())
            return;

        for (std::list<GameObject*>::const_iterator itr = lFiresInRange.begin(); itr != lFiresInRange.end(); ++itr)
        {
            (*itr)->SetRespawnTime(60);
            (*itr)->Refresh();
        }
    }

    // Custom function to check for the Generic quest trigger
    bool DoCheckQuestTrigger()
    {
        // ToDo: check if we should use the spell to check for the detonator
        if (Creature* pCredit = GetClosestCreatureWithEntry(m_creature, NPC_GENERIC_QUEST_TRIGGER, 30.0f))
        {
            m_bCheckComplete = true;
            m_creature->SetWalk(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(1, pCredit->GetPositionX(), pCredit->GetPositionY(), pCredit->GetPositionZ());

            return true;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bCheckComplete)
            ScriptedPetAI::UpdateAI(uiDiff);
    }

    void UpdatePetOOCAI(const uint32 uiDiff) override
    {
        if (m_uiCheckTimer)
        {
            if (m_uiCheckTimer <= uiDiff)
            {
                if (DoCheckQuestTrigger())
                    m_uiCheckTimer = 0;
                else
                    m_uiCheckTimer = 1000;
            }
            else
                m_uiCheckTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_living_flare(Creature* pCreature)
{
    return new npc_living_flareAI(pCreature);
}

bool EffectAuraDummy_spell_aura_dummy_living_flare(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_LIVING_FLARE_MASTER && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            pTarget->CastSpell(pTarget, SPELL_FEL_FLAREUP, TRIGGERED_OLD_TRIGGERED);

            SpellAuraHolder* pHolder = pTarget->GetSpellAuraHolder(SPELL_FEL_FLAREUP);
            if (pHolder)
            {
                if (pHolder->GetStackAmount() >= MAX_FLAREUP_STACKS)
                    pTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pTarget, pTarget);
                // Note: cosmetic aura is removed, so we need to add it back. This needs to be fixed
                else
                    pTarget->CastSpell(pTarget, SPELL_LIVING_COSMETIC, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
    return true;
}

enum
{
    QUEST_FALL_OF_MAGETHERIDON_A = 11002,
    QUEST_FALL_OF_MAGETHERIDON_H = 11003,

    YELL_1_DELAY = 2000,
    YELL_2_DELAY = 8000,

    YELL_TROLLBANE_1 = -1000590,
    YELL_TROLLBANE_2 = -1000591,

    YELL_NAZGREL_1 = -1000592,
    YELL_NAZGREL_2 = -1000593,

    SPELL_TROLLBANES_COMMAND = 39911,
    SPELL_NAZGRELS_FAVOR = 39913,

    MAX_SEARCH_DIST = 2170,

    OBJECT_MAGTHERIDONS_HEAD = 184640

    //	NPC_DANATH_TROLLBANE = 16819,
    //	NPC_NAZGREL = 3230
};

// 16819/force-commander-danath-trollbane
struct npc_danath_trollbaneAI : public ScriptedAI
{
    npc_danath_trollbaneAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool    m_bYelling = false; // if true, someone turned in the quest
    bool    m_bOnYell2 = false; // to avoid yelling the first line again

    uint32  m_uiYell1DelayRemaining = 0;
    uint32  m_uiYell2DelayRemaining = 0;

    ObjectGuid  m_guidInvoker;

    void Reset() override
    {
        m_bYelling = false;
        m_bOnYell2 = false;
        m_uiYell1DelayRemaining = 0;
        m_uiYell2DelayRemaining = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bYelling) // if yelling...
        {
            // Handle 1st yell
            if (!m_bOnYell2 && m_uiYell1DelayRemaining < uiDiff)
            {
                if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                    DoScriptText(YELL_TROLLBANE_1, m_creature, invoker);
                m_bOnYell2 = true;
            }
            else
                m_uiYell1DelayRemaining -= uiDiff;

            // Handle 2nd yell
            if (m_bOnYell2 && m_uiYell2DelayRemaining < uiDiff)
            {
                DoScriptText(YELL_TROLLBANE_2, m_creature);
                m_bYelling = false;
                m_bOnYell2 = false;

                // Mount Magtheridon's Head (update object)
                if (GameObject* goHead = GetClosestGameObjectWithEntry(m_creature, OBJECT_MAGTHERIDONS_HEAD, 100.0f))
                    if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                        goHead->Use(invoker);
            }
            else
                m_uiYell2DelayRemaining -= uiDiff;
        }

        DoMeleeAttackIfReady(); // be sure to fight back if in combat
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT && pSender == m_creature) // sanity check
            if (!m_bYelling) // don't override anything if yelling already...
            {
                m_uiYell1DelayRemaining = YELL_1_DELAY;
                m_uiYell2DelayRemaining = YELL_2_DELAY;
                m_guidInvoker = pInvoker->GetObjectGuid();
                m_bYelling = true;
            }
    }
};

bool QuestComplete_npc_trollbane(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FALL_OF_MAGETHERIDON_A)
    {
        // And trigger yelling
        pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pCreature);
    }

    return true;
}

UnitAI* GetAI_danath_trollbane(Creature* pCreature)
{
    return new npc_danath_trollbaneAI(pCreature);
}

// 3230/nazgrel
struct npc_nazgrelAI : public ScriptedAI
{
    npc_nazgrelAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool    m_bYelling = false;
    bool    m_bOnYell2 = false;

    uint32  m_uiYell1DelayRemaining = 0;
    uint32  m_uiYell2DelayRemaining = 0;

    ObjectGuid  m_guidInvoker;

    void Reset() override
    {
        m_bYelling = false;
        m_bOnYell2 = false;
        m_uiYell1DelayRemaining = 0;
        m_uiYell2DelayRemaining = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bYelling) // if yelling...
        {
            // Handle 1st yell
            if (!m_bOnYell2 && m_uiYell1DelayRemaining < uiDiff)
            {
                if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                    DoScriptText(YELL_NAZGREL_1, m_creature, invoker);
                m_bOnYell2 = true;
            }
            else
                m_uiYell1DelayRemaining -= uiDiff;

            // Handle 2nd yell
            if (m_bOnYell2 && m_uiYell2DelayRemaining < uiDiff)
            {
                DoScriptText(YELL_NAZGREL_2, m_creature);
                m_bYelling = false;
                m_bOnYell2 = false;

                // Mount Magtheridon's Head (update object)
                if (GameObject* goHead = GetClosestGameObjectWithEntry(m_creature, OBJECT_MAGTHERIDONS_HEAD, 100.0f))
                    if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                        goHead->Use(invoker);
            }
            else
                m_uiYell2DelayRemaining -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT && pSender == m_creature) // sanity check
        {
            if (!m_bYelling) // don't override anything if yelling already...
            {
                m_uiYell1DelayRemaining = YELL_1_DELAY;
                m_uiYell2DelayRemaining = YELL_2_DELAY;
                m_bYelling = true;
                m_guidInvoker = pInvoker->GetObjectGuid();
            }
        }
    }
};

// 16819/force-commander-danath-trollbane
bool QuestComplete_npc_nazgrel(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FALL_OF_MAGETHERIDON_H) // And trigger yelling        
        pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pCreature);

    return true;
}

UnitAI* GetAI_nazgrel(Creature* pCreature)
{
    return new npc_nazgrelAI(pCreature);
}

void AddSC_hellfire_peninsula()
{
    Script* pNewScript = new Script;
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
    pNewScript->GetAI = &GetAI_npc_colonel_jules;
    pNewScript->pGossipHello = &GossipHello_npc_colonel_jules;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_magister_aledis";
    pNewScript->GetAI = &GetAI_npc_magister_aledis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_flare";
    pNewScript->GetAI = &GetAI_npc_living_flare;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_living_flare;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_danath_trollbane";
    pNewScript->GetAI = &GetAI_danath_trollbane;
    pNewScript->pQuestRewardedNPC = &QuestComplete_npc_trollbane;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nazgrel";
    pNewScript->GetAI = &GetAI_nazgrel;
    pNewScript->pQuestRewardedNPC = &QuestComplete_npc_nazgrel;
    pNewScript->RegisterSelf();
}
