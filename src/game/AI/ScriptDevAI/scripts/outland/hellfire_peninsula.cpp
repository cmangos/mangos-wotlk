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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "AI/BaseAI/PetAI.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Entities/TemporarySpawn.h"
#include "Spells/SpellAuras.h"

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

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOCK);
            m_uiShockTimer = 10000;
        }
        else
            m_uiShockTimer -= uiDiff;

        if (m_uiEnvelopingWindsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ENVELOPING_WINDS);
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
    SPELL_SELF_SNARE                = 30011, // Quest - Ancestral Spirit Wolf Self Snare, casted on self shortly after spawn (purpose unknown)

    NPC_RYGA                        = 17123,

    SOUND_HOWL                      = 1018,
};

struct npc_ancestral_wolfAI : public npc_escortAI
{
    npc_ancestral_wolfAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
            Start(false, (Player*)pCreature->GetOwner());
        else
            script_error_log("npc_ancestral_wolf can not obtain owner or owner is not a player.");

        SetReactState(REACT_PASSIVE);
        Reset();
    }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ANCESTRAL_WOLF_BUFF);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateEscortAI(const uint32 /*uiDiff*/) override { };

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                DoScriptText(EMOTE_WOLF_LIFT_HEAD, m_creature);
                break;
            case 3:
                DoScriptText(EMOTE_WOLF_HOWL, m_creature);
                break;
            case 51:
                m_creature->PlayDistanceSound(SOUND_HOWL);
                Creature* pRyga = GetClosestCreatureWithEntry(m_creature, NPC_RYGA, 30.0f);
                if (pRyga && pRyga->IsAlive() && !pRyga->IsInCombat())
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
    SAY_DEMONIAC_VISITATION_END     = 20154,

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
        if (m_bIsComplete || !m_creature->IsAlive())
            return;

        if (m_uiSpawnButtressTimer <= uiDiff)
        {
            if (m_uiButtressCount >= MAX_BUTTRESS)
            {
                m_creature->CastSpell(m_creature, SPELL_SUCKER_DESPAWN_MOB, TRIGGERED_NONE);

                if (m_creature->IsInCombat())
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

struct DemoniacVisitation : public AuraScript
{
    void OnApply(Aura* aura, bool /*apply*/) const override
    {
        DoBroadcastText(SAY_DEMONIAC_VISITATION_END, aura->GetCaster(), aura->GetTarget());
    }
};

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
            case 1:
                DoScriptText(SAY_ELF_START, m_creature, pPlayer);
                break;
            case 14:
                m_creature->SetFacingTo(3.7f);
                DoScriptText(SAY_ELF_SUMMON1, m_creature, pPlayer);
                m_creature->SummonCreature(NPC_TALONGUARD, -983.206f, 4163.884f, 38.01043f, 0.72f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_TALONGUARD, -985.732f, 4157.546f, 43.50933f, 0.80f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                break;
            case 20:
                DoScriptText(SAY_ELF_RESTING, m_creature, pPlayer);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 21:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case 22:
                DoScriptText(SAY_ELF_SUMMON2, m_creature, pPlayer);
                m_creature->SummonCreature(NPC_WINDWALKER, -864.001f, 4253.191f, 43.89390f, 5.27f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);
                break;
            case 39:
                DoScriptText(SAY_ELF_COMPLETE, m_creature, pPlayer);
                break;
            case 40:
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                break;
            case 41:
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ROAD_TO_FALCON_WATCH, m_creature);
                pPlayer->GetMap()->ScriptsStart(SCRIPT_TYPE_RELAY, DBSCRIPT_END_TALERIS_INT, m_creature, m_creature);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
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
        SetReactState(REACT_PASSIVE);
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
    uint32 darknessReleasedCount;
    uint32 darknessReleasedSpawnTimer;
    std::vector<ObjectGuid> m_vSummonVector;
    CreatureList lSlimeList;

    void Reset() override
    {
        DespawnSummons();
        m_vSummonVector.clear();
        darknessReleasedCount = 0;
        darknessReleasedSpawnTimer = 40000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (darknessReleasedSpawnTimer <= uiDiff)
        {
            darknessReleasedSpawnTimer = 40000;
            if (darknessReleasedCount > 0)
                darknessReleasedCount--;
        }
        else
            darknessReleasedSpawnTimer -= uiDiff;
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
        if (pSummoned->GetEntry() == NPC_DARKNESS_RELEASED)
        {
            if (darknessReleasedCount < 3 && m_creature->HasAura(SPELL_JULES_THREATENS_AURA))
            {
                darknessReleasedCount++;
                pSummoned->AI()->SetCombatMovement(false);
                if (Creature* pBarada = GetClosestCreatureWithEntry(m_creature, NPC_ANCHORITE_BARADA, 15.0f))
                {
                    pSummoned->AI()->AttackStart(pBarada);
                    pSummoned->GetMotionMaster()->MoveRandomAroundPoint(pBarada->GetPositionX(), pBarada->GetPositionY(), pBarada->GetPositionZ(), 5.0f);
                }
            }
            else
            {
                pSummoned->ForcedDespawn();
                return;
            }
        }

        m_vSummonVector.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DARKNESS_RELEASED)
        {
            pSummoned->CastSpell(pSummoned, SPELL_FLYING_SKULL_DESPAWN, TRIGGERED_OLD_TRIGGERED);
            darknessReleasedCount--;
        }

        m_vSummonVector.erase(std::remove(m_vSummonVector.begin(), m_vSummonVector.end(), pSummoned->GetObjectGuid()), m_vSummonVector.end());
    }

    void DespawnSummons()
    {
        for (ObjectGuid& guid : m_vSummonVector)
            if (Creature* summon = m_creature->GetMap()->GetCreature(guid))
                summon->ForcedDespawn();

        lSlimeList.clear();
        GetCreatureListWithEntryInGrid(lSlimeList, m_creature, NPC_FOUL_PURGE, 40.0f);
        for (auto& itr : lSlimeList)
        {
            if (itr->IsAlive())
                itr->ForcedDespawn();
        }
    }

    void GoHome()
    {
        DespawnSummons();
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

    void JustDied(Unit* /*pKiller*/) override
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

                if (m_creature->IsAlive())
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

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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

    FACTION_ALLEDIS_FRIENDLY        = 35, // after script combat faction
    FACTION_ALLEDIS_HOSTILE         = 634, // during script combat faction

    POINT_MOVE_DISTANCE = 1,
};

enum AledisActions // order based on priority
{
    ALEDIS_LOW_HP,
    ALEDIS_ACTION_PYROBLAST,
    ALEDIS_ACTION_FROSTNOVA,
    ALEDIS_ACTION_FIREBALL,
    ALEDIS_ACTION_MAX
};

struct npc_magister_aledisAI : public CombatAI
{
    npc_magister_aledisAI(Creature* creature) : CombatAI(creature, ALEDIS_ACTION_MAX)
    {
        AddTimerlessCombatAction(ALEDIS_LOW_HP, true);
        AddCombatAction(ALEDIS_ACTION_PYROBLAST, 10000, 14000);
        AddCombatAction(ALEDIS_ACTION_FROSTNOVA, 3000, 9000);
        AddCombatAction(ALEDIS_ACTION_FIREBALL, 1000u);
        AddDistanceSpell(SPELL_FROST_NOVA);
        SetRangedMode(true, 20.f, TYPE_PROXIMITY);
        Reset();
    }

    bool m_bIsDefeated;
    bool m_bAllyAttacker;

    void Reset() override
    {
        CombatAI::Reset();

        m_bAllyAttacker = false;
        m_bIsDefeated = false;

        SetCombatMovement(true);
        SetCombatScriptStatus(false);

        SetDeathPrevention(false);

        SetReactState(REACT_AGGRESSIVE);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // TODO: check if needs to be removed somewhere
    }

    uint32 GetSubsequentActionTimer(AledisActions id)
    {
        switch (id)
        {
            case ALEDIS_ACTION_PYROBLAST: return urand(18000, 21000);
            case ALEDIS_ACTION_FROSTNOVA: return urand(12000, 16000);
            case ALEDIS_ACTION_FIREBALL: return urand(3000, 4000);
            default: return 0;
        }
    }

    void Aggro(Unit* /*who*/)
    {
        if (m_creature->GetFaction() == FACTION_ALLEDIS_HOSTILE)
            SetDeathPrevention(true);
    }

    void EvadeReset()
    {
        m_bAllyAttacker = false;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (!m_bIsDefeated)
            m_creature->LoadCreatureAddon(true);

        if (m_creature->IsAlive())
        {
            if (!m_bIsDefeated)
            {
                m_creature->SetWalk(true);
                m_creature->GetMotionMaster()->MoveWaypoint();
            }
            else
            {
                m_creature->GetMotionMaster()->MoveIdle();
                EvadeReset();
            }
        }

        m_creature->SetLootRecipient(nullptr);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ALEDIS_LOW_HP:
            {
                if (m_creature->GetHealthPercent() > 20.0f || m_creature->GetFaction() != FACTION_ALLEDIS_HOSTILE)
                    return;

                // evade when defeated; faction is reset automatically
                m_bIsDefeated = true;
                m_creature->SetFactionTemporary(FACTION_ALLEDIS_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
                EnterEvadeMode();
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                SetReactState(REACT_PASSIVE);

                DoScriptText(SAY_ALEDIS_DEFEAT, m_creature);
                m_creature->ForcedDespawn(30000);
                return;
            }
            case ALEDIS_ACTION_PYROBLAST:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PYROBLAST) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(AledisActions(action)));
                return;
            }
            case ALEDIS_ACTION_FROSTNOVA:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_FROST_NOVA, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                    if (DoCastSpellIfCan(m_creature, SPELL_FROST_NOVA) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(AledisActions(action)));
                return;
            }
            case ALEDIS_ACTION_FIREBALL:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(AledisActions(action)));
                return;
            }
        }
    }
};

enum
{
    NPC_LIVING_FLARE            = 24916,
    NPC_GENERIC_QUEST_TRIGGER_LAB = 24959,
    NPC_UNSTABLE_LIVING_FLARE   = 24958,

    GO_LARGE_FIRE               = 187084,

    SPELL_LIVING_FLARE_MASTER   = 44877,
    SPELL_FEL_FLAREUP           = 44944,
    SPELL_TRANSFORM             = 44943,
    SPELL_QUEST_CREDIT          = 44947,
    SPELL_DETONATOR             = 44948,
    SPELL_LIVING_COSMETIC       = 44880,
    SPELL_UNSTABLE_COSMETIC     = 46196,
    SPELL_HUGE_EXPLOSION_TEST   = 46225,

    EMOTE_LIVING_FLARE          = -1001214,
    EMOTE_UNSTABLE_FLARE        = -1001215,
};

struct npc_living_flareAI : public ScriptedPetAI
{
    npc_living_flareAI(Creature* pCreature) : ScriptedPetAI(pCreature), m_uiStacks(0)
    {
        Reset();
    }

    uint32 m_uiStacks;
    uint32 m_uiCheckTimer;
    bool m_uiFollow;
    uint32 m_uiExplosionTimer;
    ObjectGuid m_uiQuestCreditGuid;

    void Reset() override
    {
        m_uiCheckTimer = 0;
        m_uiFollow = true;
    }

    void JustRespawned() override
    {
        m_creature->CastSpell(nullptr, SPELL_LIVING_COSMETIC, TRIGGERED_OLD_TRIGGERED);
        SetReactState(REACT_PASSIVE);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_FEL_FLAREUP) // handle morph here instead of spellEffects so we can do script text
        {
            m_uiStacks++;
            if (m_uiStacks == 8)
            {
                m_creature->CastSpell(nullptr, SPELL_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
                DoScriptText(EMOTE_LIVING_FLARE, m_creature);
                m_uiCheckTimer = 1000;
            }
        }
    }

    void MovementInform(uint32 /*uiMovementType*/, uint32 uiPointId) override
    {
        if (uiPointId == 1)
        {
            m_uiExplosionTimer = 4000;
            m_creature->CastSpell(nullptr, SPELL_QUEST_CREDIT, TRIGGERED_OLD_TRIGGERED); // is cast at pet owner
            DoScriptText(EMOTE_UNSTABLE_FLARE, m_creature);
            if (Creature* questTrigger = GetClosestCreatureWithEntry(m_creature, NPC_GENERIC_QUEST_TRIGGER_LAB, 100.0f))
                questTrigger->CastSpell(questTrigger, SPELL_DETONATOR, TRIGGERED_OLD_TRIGGERED);
            m_creature->RemoveAurasDueToSpell(SPELL_UNSTABLE_COSMETIC);
        }
    }

    void CheckPortalDist()
    {
        if (Creature* questCredit = GetClosestCreatureWithEntry(m_creature, NPC_GENERIC_QUEST_TRIGGER_LAB, 50.f)) // if within distance of portal, initiate final sequence
        {
            m_uiCheckTimer = 0;
            m_uiFollow = false;
            m_creature->GetMotionMaster()->MovePoint(1, 831.892f, 2511.94f, 292.0167f);
            m_uiQuestCreditGuid = questCredit->GetObjectGuid();
        }
        else
            m_uiCheckTimer = 1000;
    }

    void UpdatePetAI(const uint32 uiDiff) override
    {
        if (m_uiCheckTimer)
        {
            if (m_uiCheckTimer <= uiDiff)
                CheckPortalDist();
            else
                m_uiCheckTimer -= uiDiff;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiFollow)
        {
            Unit* pOwner = m_creature->GetMaster();

            if (!pOwner)
                return;

            if (m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
            {
                // not following, so start follow
                if (!m_creature->hasUnitState(UNIT_STAT_FOLLOW))
                    m_creature->GetMotionMaster()->MoveFollow(pOwner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

                // update when not in combat
                UpdatePetAI(uiDiff);
            }
        }
        if (m_uiExplosionTimer)
        {
            if (m_uiExplosionTimer <= uiDiff)
            {
                m_uiExplosionTimer = 0;
                Unit* master = m_creature->GetMaster();
                if (master && master->GetTypeId() == TYPEID_PLAYER)
                {
                    m_creature->ForcedDespawn();
                    ((Player*)master)->RemoveMiniPet();
                }
            }
            else
                m_uiExplosionTimer -= uiDiff;
        }
    }
};

struct LivingFlareDetonator : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            target->CastSpell(nullptr, SPELL_HUGE_EXPLOSION_TEST, TRIGGERED_OLD_TRIGGERED);
        GameObjectList objectList;
        GetGameObjectListWithEntryInGrid(objectList, spell->GetCaster(), GO_LARGE_FIRE, 30.f);
        for (auto* flame : objectList)
        {
            flame->SetLootState(GO_READY);
            flame->SetRespawnTime(60);           // despawn object in 60 seconds
            flame->Refresh();
        }
    }
};

struct LivingFlareMaster : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            if (aura->GetTarget()->GetAuraCount(SPELL_FEL_FLAREUP) < 8)
                aura->GetTarget()->CastSpell(nullptr, SPELL_FEL_FLAREUP, TRIGGERED_OLD_TRIGGERED);
    }
};

struct LivingFlareUnstable : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            if (target->IsCreature())
                static_cast<Creature*>(target)->UpdateEntry(NPC_UNSTABLE_LIVING_FLARE);
            target->RemoveAurasDueToSpell(SPELL_LIVING_COSMETIC);
            target->CastSpell(nullptr, SPELL_UNSTABLE_COSMETIC, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

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
                if (GameObject* goHead = GetClosestGameObjectWithEntry(m_creature, OBJECT_MAGTHERIDONS_HEAD, 120.0f))
                    if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                        goHead->Use(invoker);
            }
            else
                m_uiYell2DelayRemaining -= uiDiff;
        }

        ScriptedAI::UpdateAI(uiDiff);
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

bool QuestComplete_npc_trollbane(Player* player, Creature* creature, const Quest* quest)
{
    if (quest->GetQuestId() == QUEST_FALL_OF_MAGETHERIDON_A)
    {
        // And trigger yelling
        creature->AI()->SendAIEvent(AI_EVENT_START_EVENT, player, creature);
        return true;
    }

    return false;
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
                if (GameObject* goHead = GetClosestGameObjectWithEntry(m_creature, OBJECT_MAGTHERIDONS_HEAD, 120.0f))
                    if (Unit* invoker = m_creature->GetMap()->GetUnit(m_guidInvoker))
                        goHead->Use(invoker);
            }
            else
                m_uiYell2DelayRemaining -= uiDiff;
        }

        ScriptedAI::UpdateAI(uiDiff);
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
bool QuestComplete_npc_nazgrel(Player* player, Creature* creature, const Quest* quest)
{
    if (quest->GetQuestId() == QUEST_FALL_OF_MAGETHERIDON_H) // And trigger yelling        
    {
        creature->AI()->SendAIEvent(AI_EVENT_START_EVENT, player, creature);
        return true;
    }

    return false;
}

UnitAI* GetAI_nazgrel(Creature* pCreature)
{
    return new npc_nazgrelAI(pCreature);
}

/*
#############
## npc_vindicator_sedai
#############
*/

enum
{
    QUEST_SEERS_RELIC = 9545,

    NPC_SEDAI         = 17404,
    NPC_SEDAI_CORPSE  = 16852,
    NPC_MAGHAR_ESCORT = 17417,
    NPC_FEL_ORC       = 17418,
    NPC_KRUN          = 17405,
    NPC_SEDAI_QCM     = 17413,

    SAY_EVENT_MAGHAR_ESCORT = -1015066,
    SAY_EVENT_SEDAI_1       = -1015067,
    SAY_EVENT_SEDAI_2       = -1015068,
    SAY_EVENT_SEDAI_3       = -1015069,
    SAY_EVENT_SEDAI_4       = -1015070,
    SAY_EVENT_KRUN          = -1015071,

    SPELL_EXECUTE_SEDAI     = 30462,
    SPELL_HOLY_FIRE         = 17141,
    SPELL_HAMMER_OF_JUSTICE = 13005,
    SPELL_KICK              = 30460
};

enum SedaiActions : uint32
{
    SEDAI_COMBAT_ACTION_HOLYFIRE,
    SEDAI_COMBAT_ACTION_HAMMER,
    SEDAI_COMBAT_ACTION_MAX,

    SEDAI_ACTION_FACE_ESCORT,
    SEDAI_ACTION_ESCORT_KICK,
    SEDAI_ACTION_ESCORT_SAY,
    SEDAI_ACTION_SEDAI_KNEEL,
    SEDAI_ACTION_ESCORTS_MOVE_1,
    SEDAI_ACTION_FELORC_SPAWN_ATTACK,
    SEDAI_ACTION_QUEST_COMPLETE,
    SEDAI_ACTION_SEDAI_MOVE_2,
    SEDAI_ACTION_SEDAI_START_ATTACK,
};

struct npc_vindicator_sedaiAI : public ScriptedAI
{
    npc_vindicator_sedaiAI(Creature* creature) : ScriptedAI(creature, SEDAI_COMBAT_ACTION_MAX)
    {
        m_creature->SetActiveObjectState(true);
        SetReactState(REACT_DEFENSIVE);

        AddCombatAction(SEDAI_COMBAT_ACTION_HOLYFIRE, 0u);
        AddCombatAction(SEDAI_COMBAT_ACTION_HAMMER, 0u);

        AddCustomAction(SEDAI_ACTION_FACE_ESCORT, true, [&]()
        {
            if (Creature* maghar = m_creature->GetMap()->GetCreature(m_maghar))
                m_creature->SetFacingToObject(maghar);

            ResetTimer(SEDAI_ACTION_ESCORT_KICK, 2000u);
        });

        AddCustomAction(SEDAI_ACTION_ESCORT_KICK, true, [&]()
        {
            if (Creature* corpse = GetClosestCreatureWithEntry(m_creature, NPC_SEDAI_CORPSE, 10.0f, false))
            {
                corpse->GetPosition(m_positionCorpse[0], m_positionCorpse[1], m_positionCorpse[2]);
                m_creature->SetFacingToObject(corpse);
            }

            if (Creature* maghar = m_creature->GetMap()->GetCreature(m_maghar))
                maghar->AI()->DoCastSpellIfCan(m_creature, SPELL_KICK);

            ResetTimer(SEDAI_ACTION_ESCORT_SAY, 2000);
        });

        AddCustomAction(SEDAI_ACTION_ESCORT_SAY, true, [&]()
        {
            if (Creature* maghar = m_creature->GetMap()->GetCreature(m_magharTwo))
                DoScriptText(SAY_EVENT_MAGHAR_ESCORT, maghar);
            ResetTimer(SEDAI_ACTION_SEDAI_KNEEL, 1000);
        });

        AddCustomAction(SEDAI_ACTION_SEDAI_KNEEL, true, [&]()
        {
            m_creature->SetSheath(SHEATH_STATE_UNARMED);
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
            ResetTimer(SEDAI_ACTION_ESCORTS_MOVE_1, 1000);
        });

        AddCustomAction(SEDAI_ACTION_ESCORTS_MOVE_1, true, [&]()
        {
            Map* map = m_creature->GetMap();
            if (Creature* maghar = map->GetCreature(m_maghar))
                maghar->GetMotionMaster()->MovePoint(2, 218.7385f, 4128.742f, 81.00686f);
            if (Creature* maghar = map->GetCreature(m_magharTwo))
                maghar->GetMotionMaster()->MovePoint(2, 219.5054f, 4125.231f, 81.05459f);
            ResetTimer(SEDAI_ACTION_FELORC_SPAWN_ATTACK, 3000);
        });

        AddCustomAction(SEDAI_ACTION_FELORC_SPAWN_ATTACK, true, [&]()
        {
            if (Creature* orc = m_creature->SummonCreature(NPC_FEL_ORC, 258.168854f, 4109.307617f, 91.639290f, 2.644194f, TEMPSPAWN_TIMED_DESPAWN, 60000, true))
            {
                m_felOrc = orc->GetObjectGuid();
                if (Creature* maghar = m_creature->GetMap()->GetCreature(m_maghar))
                {
                    orc->AI()->AttackStart(maghar);
                    maghar->AI()->AttackStart(orc);
                }
            }
            if (Creature* orc = m_creature->SummonCreature(NPC_FEL_ORC, 256.429932f, 4105.590820f, 90.982086f, 2.734515f, TEMPSPAWN_TIMED_DESPAWN, 60000, true))
            {
                m_felOrcTwo = orc->GetObjectGuid();
                if (Creature* maghar = m_creature->GetMap()->GetCreature(m_magharTwo))
                {
                    orc->AI()->AttackStart(maghar);
                    maghar->AI()->AttackStart(orc);
                }
            }
            ResetTimer(SEDAI_ACTION_QUEST_COMPLETE, 1000);
        });

        AddCustomAction(SEDAI_ACTION_QUEST_COMPLETE, true, [&]()
        {
            if (Player* player = (Player*)m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
            {
                if (Creature* qcm = GetClosestCreatureWithEntry(m_creature, NPC_SEDAI_QCM, 30.0f))
                {
                    player->RewardPlayerAndGroupAtEventCredit(qcm->GetEntry(), qcm);
                    player->RewardPlayerAndGroupAtEventExplored(QUEST_SEERS_RELIC, qcm);
                }
                DoScriptText(SAY_EVENT_SEDAI_1, m_creature);
                ResetTimer(SEDAI_ACTION_SEDAI_MOVE_2, 6000);
            }
        });

        AddCustomAction(SEDAI_ACTION_SEDAI_MOVE_2, true, [&]()
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            DoScriptText(SAY_EVENT_SEDAI_2, m_creature);
            m_creature->GetMotionMaster()->MovePoint(2, 202.1543f, 4138.074f, 76.15149f);
        });

        AddCustomAction(SEDAI_ACTION_SEDAI_START_ATTACK, true, [&]()
        {
            ResetTimer(SEDAI_COMBAT_ACTION_HOLYFIRE, 25000);
            ResetTimer(SEDAI_COMBAT_ACTION_HAMMER, 15000);

            if (Creature* felOrc = m_creature->GetMap()->GetCreature(m_felOrcTwo))
                m_creature->AI()->AttackStart(felOrc);
        });
    }

    ObjectGuid m_maghar;
    ObjectGuid m_magharTwo;
    ObjectGuid m_felOrc;
    ObjectGuid m_felOrcTwo;
    ObjectGuid m_krun;

    float m_positionCorpse[3];

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        if (Creature* maghar = m_creature->SummonCreature(NPC_MAGHAR_ESCORT, 216.0346f, 4125.61f, 80.22345f, 3.41032f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 300000, true))
        {
            maghar->SetCorpseDelay(20);
            m_maghar = maghar->GetObjectGuid();
            maghar->SetMaxHealth(1);
            maghar->GetMotionMaster()->MovePoint(1, 199.061f, 4142.329f, 75.14999f);
            maghar->GetMotionMaster()->MoveIdle();
        }
        if (Creature* maghar = m_creature->SummonCreature(NPC_MAGHAR_ESCORT, 214.1331f, 4123.221f, 79.6478f, 2.233855f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 300000, true))
        {
            maghar->SetCorpseDelay(20);
            m_magharTwo = maghar->GetObjectGuid();
            maghar->GetMotionMaster()->MovePoint(1, 196.8983f, 4140.25f, 74.4968f);
            maghar->GetMotionMaster()->MoveIdle();
        }

        m_creature->GetMotionMaster()->MovePoint(1, 196.6698f, 4143.903f, 74.37656f);
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < SEDAI_COMBAT_ACTION_MAX; ++i)
        {
            if (!GetActionReadyStatus(i))
                continue;

            switch (i)
            {
                case SEDAI_COMBAT_ACTION_HAMMER:
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HAMMER_OF_JUSTICE) == CAST_OK)
                    {
                        SetActionReadyStatus(i, false);
                        ResetTimer(i, 15000);
                        return;
                    }
                    continue;
                case SEDAI_COMBAT_ACTION_HOLYFIRE:
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HOLY_FIRE) == CAST_OK)
                    {
                        SetActionReadyStatus(i, false);
                        ResetTimer(i, 25000);
                        return;
                    }
                    continue;
            }
        }
    }

    void MovementInform(uint32 /*movementType*/, uint32 data) override
    {
        switch (data)
        {
            case 1:
            {
                m_creature->GetMotionMaster()->MoveIdle();
                ResetTimer(SEDAI_ACTION_FACE_ESCORT, 0);
                break;
            }
            case 2:
            {
                if (Creature* felOrc = m_creature->GetMap()->GetCreature(m_felOrc))
                    m_creature->SetFacingToObject(felOrc);

                DoScriptText(SAY_EVENT_SEDAI_3, m_creature);
                ResetTimer(SEDAI_ACTION_SEDAI_START_ATTACK, 1000);
                break;
            }
            case 3:
            {
                if (Creature* krun = m_creature->SummonCreature(NPC_KRUN, 225.445526f, 4120.721191f, 82.345039f, 2.443936f, TEMPSPAWN_TIMED_DESPAWN, 60000, true))
                {
                    m_krun = krun->GetObjectGuid();
                    krun->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    krun->SetWalk(false, true);
                    krun->GetMotionMaster()->MovePoint(1, 202.904419f, 4138.468262f, 76.176216f);
                }
                DoScriptText(SAY_EVENT_SEDAI_4, m_creature);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->SetSheath(SHEATH_STATE_UNARMED);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* krun = m_creature->GetMap()->GetCreature(m_krun))
            m_creature->SetFacingToObject(krun);

        if (Creature* qcm = GetClosestCreatureWithEntry(m_creature, NPC_SEDAI_QCM, 30.0f))
            qcm->ForcedDespawn();
    }

    void JustReachedHome() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->GetMotionMaster()->MovePoint(3, m_positionCorpse[0], m_positionCorpse[1], m_positionCorpse[2]);
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();

        ExecuteActions();
    }
};

UnitAI* GetAI_npc_vindicator_sedai(Creature* creature)
{
    return new npc_vindicator_sedaiAI(creature);
}

/*
##############
## npc_krun
##############
*/

enum KrunActions : uint32
{
    KRUN_ACTION_EXECUTE_SEDAI,
    KRUN_ACTION_LAUGH,
    KRUN_ACTION_DESPAWN
};

struct npc_krunAI : public ScriptedAI
{
    npc_krunAI(Creature* creature) : ScriptedAI(creature)
    {
        AddCustomAction(KRUN_ACTION_EXECUTE_SEDAI, true, [&]()
        {
            DoScriptText(SAY_EVENT_KRUN, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_EXECUTE_SEDAI);
            ResetTimer(KRUN_ACTION_LAUGH, 2000);
        });

        AddCustomAction(KRUN_ACTION_LAUGH, true, [&]()
        {
            m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);
            ResetTimer(KRUN_ACTION_DESPAWN, 2000);
        });

        AddCustomAction(KRUN_ACTION_DESPAWN, true, [&]()
        {
            if (TemporarySpawn* summon = (TemporarySpawn*)m_creature)
                summon->UnSummon();
        });

        SetReactState(REACT_PASSIVE);
    }

    void MovementInform(uint32 /*movementType*/, uint32 data) override
    {
        switch (data)
        {
            case 1:
                m_creature->SetWalk(true, true);
                m_creature->GetMotionMaster()->MovePoint(2, 193.358658f, 4149.128906f, 73.768143f);
                break;
            case 2:
                m_creature->GetMotionMaster()->MoveIdle();
                ResetTimer(KRUN_ACTION_EXECUTE_SEDAI, 1000);
                break;
        }
    }
};

UnitAI* GetAI_npc_krun(Creature* creature)
{
    return new npc_krunAI(creature);
}

/*
###############
## npc_laughing_skull
###############
*/

struct npc_laughing_skullAI : public ScriptedAI
{
    npc_laughing_skullAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_DEFENSIVE);
        creature->GetCombatManager().SetLeashingDisable(true);
        Reset();
    }

    void Reset() override {}

    void JustReachedHome() override
    {
        m_creature->GetMotionMaster()->MoveIdle();
    }
};

UnitAI* GetAI_npc_laughing_skull(Creature* creature)
{
    return new npc_laughing_skullAI(creature);
}

/*
###############
## npc_maghar_escort
###############
*/

struct npc_maghar_escortAI : public ScriptedAI
{
    npc_maghar_escortAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {

    }

    void EnterEvadeMode() override
    {
        if (!GetClosestCreatureWithEntry(m_creature, NPC_FEL_ORC, 60.0f))
        {
            if (TemporarySpawn* summon = (TemporarySpawn*)m_creature)
                summon->UnSummon();
        }
    }

    void JustReachedHome() override
    {
        m_creature->GetMotionMaster()->MoveIdle();
    }
};

UnitAI* GetAI_npc_maghar_escort(Creature* creature)
{
    return new npc_maghar_escortAI(creature);
}

bool ProcessEventId_sedai_vision(uint32 /*eventId*/, Object* source, Object* /*target*/, bool /*isStart*/)
{
    if (Creature* sedai = GetClosestCreatureWithEntry((WorldObject*)source, NPC_SEDAI, 100.0f))
        return false;
    else
    {
        Player* player = (Player*)source;
        sedai = player->SummonCreature(17404, 211.1362f, 4126.989f, 78.81913f, 2.281034f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 300000, true);
        sedai->SetCorpseDelay(20);
        return true;
    }
}

struct KaliriNest : public GameObjectAI
{
    using GameObjectAI::GameObjectAI;
    void OnLootStateChange(Unit* /*user*/) override
    {
        if (m_go->GetLootState() == GO_JUST_DEACTIVATED)
        {
            m_go->SetForcedDespawn();
            if (GameObjectData const* data = sObjectMgr.GetGOData(m_go->GetObjectGuid().GetCounter()))
                m_go->SetRespawnDelay(data->GetRandomRespawnTime());
        }
    }
};

enum
{
    NPC_GRYPHON_BRIGADIER_SOUTH         = 21170,
    NPC_GRYPHON_BRIGADIER_NORTH         = 22404,
    NPC_GRYPHON_BRIGADIER_FORGE         = 22405,
    NPC_GRYPHON_BRIGADIER_FOOTHILL      = 22406,

    NPC_CREDIT_MARKER_SOUTH             = 21182,
    NPC_CREDIT_MARKER_NORTH             = 22401,
    NPC_CREDIT_MARKER_FORGE             = 22402,
    NPC_CREDIT_MARKER_FOOTHILL          = 22403,

    NPC_CREDIT_MARKER_THEY_MUST_BURN    = 21173, // summons the Gryphon NPCs - 2 spawn locations

    SPELL_SUMMON_BRIGADIER_SOUTH        = 36302,
    SPELL_SUMMON_BRIGADIER_NORTH        = 39106,
    SPELL_SUMMON_BRIGADIER_FORGE        = 39107,
    SPELL_SUMMON_BRIGADIER_FOOTHILL     = 39108,

    COUNT_SPAWNS                        = 4,
};

struct go_smoke_beacon : public GameObjectAI
{
    go_smoke_beacon(GameObject* go) : GameObjectAI(go) { m_uiCustomAnimTimer = 250; m_uiAnimCount = 0; }

    uint32 m_uiCustomAnimTimer;
    uint8 m_uiAnimCount;

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_go->IsSpawned())
            return;

        if (m_uiAnimCount < 8)
        {
            if (m_uiCustomAnimTimer <= uiDiff)
            {
                m_go->SendGameObjectCustomAnim(m_go->GetObjectGuid(), 3);
                m_uiCustomAnimTimer = 1000;
                m_uiAnimCount++;
            }
            else
                m_uiCustomAnimTimer -= uiDiff;
        }
    }
};

GameObjectAI* GetAI_go_smoke_beacon(GameObject* go)
{
    return new go_smoke_beacon(go);
}

struct npc_credit_marker_they_must_burnAI : public ScriptedAI
{
    npc_credit_marker_they_must_burnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiSummonTimer;
    uint32 m_uiSpawnCounter;
    uint32 m_uiCreditMarker;

    void Reset() override
    {
        m_uiSummonTimer = 0;
        m_uiCreditMarker = 0;
        m_uiSpawnCounter = 0;
    }

    void SummonBombers(uint32 creditMarker)
    {
        m_uiSummonTimer = 1000;
        m_uiCreditMarker = creditMarker;
        m_uiSpawnCounter = 0;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_GRYPHON_BRIGADIER_SOUTH:
            case NPC_GRYPHON_BRIGADIER_FOOTHILL:
                pSummoned->GetMotionMaster()->MoveWaypoint(urand(0, 3));
                break;
            case NPC_GRYPHON_BRIGADIER_NORTH:
                pSummoned->GetMotionMaster()->MoveWaypoint(urand(0, 2));
                break;
            case NPC_GRYPHON_BRIGADIER_FORGE:
                pSummoned->GetMotionMaster()->MoveWaypoint();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSpawnCounter < COUNT_SPAWNS)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                switch (m_uiCreditMarker)
                {
                    case NPC_CREDIT_MARKER_SOUTH:
                        m_creature->CastSpell(nullptr, SPELL_SUMMON_BRIGADIER_SOUTH, TRIGGERED_OLD_TRIGGERED);
                        break;
                    case NPC_CREDIT_MARKER_NORTH:
                        m_creature->CastSpell(nullptr, SPELL_SUMMON_BRIGADIER_NORTH, TRIGGERED_OLD_TRIGGERED);
                        break;
                    case NPC_CREDIT_MARKER_FORGE:
                        m_creature->CastSpell(nullptr, SPELL_SUMMON_BRIGADIER_FORGE, TRIGGERED_OLD_TRIGGERED);
                        break;
                    case NPC_CREDIT_MARKER_FOOTHILL:
                        m_creature->CastSpell(nullptr, SPELL_SUMMON_BRIGADIER_FOOTHILL, TRIGGERED_OLD_TRIGGERED);
                        break;
                }
                m_uiSummonTimer = 1000;
                m_uiSpawnCounter++;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }
        else
        {
            Reset();
        }
    }
};

UnitAI* GetAI_npc_credit_marker_they_must_burn(Creature* pCreature)
{
    return new npc_credit_marker_they_must_burnAI(pCreature);
}

struct SummonSmokeBeacon : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            if (Creature* summoner = GetClosestCreatureWithEntry(unitTarget, NPC_CREDIT_MARKER_THEY_MUST_BURN, 250.f))
            {
                if (npc_credit_marker_they_must_burnAI* summonerAI = dynamic_cast<npc_credit_marker_they_must_burnAI*>(summoner->AI()))
                {
                    summonerAI->SummonBombers(unitTarget->GetEntry());
                }
            }
        }
    }
};

enum
{
    FACTION_SCARAB_HOSTILE = 14,
};

struct CursedScarabPeriodicTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (target && target->GetFaction() != FACTION_SCARAB_HOSTILE && urand(0, 10) == 0)
            target->setFaction(FACTION_SCARAB_HOSTILE);
    }
};

struct CursedScarabDespawnPeriodicTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (target && target->IsCreature())
            static_cast<Creature*>(target)->ForcedDespawn();
    }
};

enum
{
    SPELL_EXPOSE_RAZORTHORN_ROOT = 44935,
    SPELL_SUMMON_RAZORTHORN_ROOT = 44941,
    NPC_RAZORTHORN_RAVAGER       = 24922,
    GO_RAZORTHORN_DIRT_MOUND     = 187073,

    SPELL_REND = 13443,
    SPELL_RAVAGE = 33781,

    POINT_MOUND = 1,
};

struct ExposeRazorthornRoot : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->AI())
            return;

        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
    }
};

struct npc_razorthorn_ravager_pet : public PetAI
{
    npc_razorthorn_ravager_pet(Creature* creature) : PetAI(creature), m_animStage(0)
    {
        AddCustomAction(1, true, [&]() { HandleAnimations(); });
    }

    uint32 m_animStage;
    ObjectGuid m_mound;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && m_animStage == 0)
            HandleAnimations();
    }

    void MovementInform(uint32 movementType, uint32 data)
    {
        if (movementType == POINT_MOTION_TYPE)
            if (data == POINT_MOUND)
                ResetTimer(1, 500);
    }

    void HandleAnimations()
    {
        uint32 timer = 0;
        switch (m_animStage)
        {
            case 0:
            {
                GameObject* mound = GetClosestGameObjectWithEntry(m_creature, GO_RAZORTHORN_DIRT_MOUND, 20.0f);
                if (!mound || mound->GetRespawnTime() != 0)
                {
                    m_animStage = 0;
                    return;
                }

                m_mound = mound->GetObjectGuid();
                float x, y, z;
                mound->GetContactPoint(mound, x, y, z, CONTACT_DISTANCE);
                m_creature->SendPetTalk(0);
                m_creature->GetMotionMaster()->MovePoint(POINT_MOUND, x, y, z);
                SetCombatScriptStatus(true);
                break;
            }
            case 1:
                m_creature->CastSpell(nullptr, SPELL_SUMMON_RAZORTHORN_ROOT, TRIGGERED_OLD_TRIGGERED);
                if (GameObject* mound = m_creature->GetMap()->GetGameObject(m_mound))
                    mound->SetLootState(GO_JUST_DEACTIVATED);
                else
                {
                    m_animStage = 0;
                    SetCombatScriptStatus(false);
                    return;
                }
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
                timer = 2000;
                break;
            case 2:
                m_animStage = 0;
                SetCombatScriptStatus(false);
                m_unit->GetMotionMaster()->MoveFollow(m_creature->GetCharmer(), PET_FOLLOW_ANGLE, PET_FOLLOW_DIST, true);
                return;
        }
        ++m_animStage;
        if (timer)
            ResetTimer(1, timer);
    }
};

enum RazorthornActions
{
    RAZORTHORN_REND,
    RAZORTHORN_RAVAGE,
    RAZORTHORN_ACTION_MAX,
};

struct npc_razorthorn_ravager : public CombatAI
{
    npc_razorthorn_ravager(Creature* creature) : CombatAI(creature, RAZORTHORN_ACTION_MAX)
    {
        AddCombatAction(RAZORTHORN_REND, 10000, 15000);
        AddCombatAction(RAZORTHORN_RAVAGE, 5000, 9000);
    }

    bool m_charmed;

    void Reset() override
    {

    }

    bool CanHandleCharm() override { return true; }

    void JustGotCharmed(Unit* /*charmer*/) override
    {
        if (CharmInfo* charmInfo = m_creature->GetCharmInfo())
            charmInfo->SetCharmState(new npc_razorthorn_ravager_pet(m_creature));
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case RAZORTHORN_REND:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_REND) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
            case RAZORTHORN_RAVAGE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAVAGE) == CAST_OK)
                    ResetCombatAction(action, urand(8000, 12000));
                break;
        }
    }
};

struct CharmRavager : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_1)
            if (aura->GetTarget()->IsCreature())
                static_cast<Creature*>(aura->GetTarget())->ForcedDespawn();
    }
};

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
    pNewScript->GetAI = &GetNewAIInstance<npc_magister_aledisAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_flare";
    pNewScript->GetAI = &GetNewAIInstance<npc_living_flareAI>;
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

    pNewScript = new Script;
    pNewScript->Name = "npc_vindicator_sedai";
    pNewScript->GetAI = &GetAI_npc_vindicator_sedai;
    pNewScript->pProcessEventId = &ProcessEventId_sedai_vision;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_krun";
    pNewScript->GetAI = &GetAI_npc_krun;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_laughing_skull";
    pNewScript->GetAI = &GetAI_npc_laughing_skull;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_maghar_escort";
    pNewScript->GetAI = &GetAI_npc_maghar_escort;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_kaliri_nest";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<KaliriNest>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_smoke_beacon";
    pNewScript->GetGameObjectAI = &GetAI_go_smoke_beacon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_credit_marker_they_must_burn";
    pNewScript->GetAI = &GetAI_npc_credit_marker_they_must_burn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_razorthorn_ravager";
    pNewScript->GetAI = &GetNewAIInstance<npc_razorthorn_ravager>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonSmokeBeacon>("spell_summon_smoke_beacon");
    RegisterSpellScript<CursedScarabPeriodicTrigger>("spell_cursed_scarab_periodic");
    RegisterSpellScript<CursedScarabDespawnPeriodicTrigger>("spell_cursed_scarab_despawn_periodic");
    RegisterSpellScript<ExposeRazorthornRoot>("spell_razorthorn_root");
    RegisterSpellScript<CharmRavager>("spell_charm_ravager");
    RegisterSpellScript<LivingFlareDetonator>("spell_living_flare_detonator");
    RegisterSpellScript<LivingFlareMaster>("spell_living_flare_master");
    RegisterSpellScript<LivingFlareUnstable>("spell_living_flare_unstable");
    RegisterSpellScript<DemoniacVisitation>("spell_demoniac_visitation");
}
