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
SDName: Zuldrak
SD%Complete: 100
SDComment: Quest support: 12652, 12934.
SDCategory: Zuldrak
EndScriptData */

/* ContentData
npc_gurgthock
npc_ghoul_feeding_bunny
npc_decaying_ghoul
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Entities/TemporarySpawn.h"

/*######
## npc_gurgthock
######*/

enum
{
    QUEST_FROM_BEYOND = 12934,

    NPC_AZBARIN       = 30026,
    NPC_DUKE_SINGEN   = 30019,
    NPC_ERATHIUS      = 30025,
    NPC_GARGORAL      = 30024
};

static float m_afSpawnLocation[] = {5768.71f, -2969.29f, 273.816f};
static uint32 m_auiBosses[] = {NPC_AZBARIN, NPC_DUKE_SINGEN, NPC_ERATHIUS, NPC_GARGORAL};

struct npc_gurgthockAI : public ScriptedAI
{
    npc_gurgthockAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    void SetPlayer(Player* pPlayer)
    {
        m_playerGuid = pPlayer->GetObjectGuid();
    }

    void Reset() override
    {
        m_playerGuid.Clear();
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        uint32 uiEntry = pSummoned->GetEntry();
        for (unsigned int m_auiBosse : m_auiBosses)
        {
            if (uiEntry == m_auiBosse)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_FROM_BEYOND, m_creature);

                m_playerGuid.Clear();
                return;
            }
        }
    }
};

bool QuestAccept_npc_gurgthock(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FROM_BEYOND)
    {
        pCreature->SummonCreature(m_auiBosses[urand(0, 3)], m_afSpawnLocation[0], m_afSpawnLocation[1], m_afSpawnLocation[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);

        if (npc_gurgthockAI* pGurthockAI = dynamic_cast<npc_gurgthockAI*>(pCreature->AI()))
            pGurthockAI->SetPlayer(pPlayer);
    }
    return true;
}

UnitAI* GetAI_npc_gurgthock(Creature* pCreature)
{
    return new npc_gurgthockAI(pCreature);
}

/*######
## npc_ghoul_feeding_bunny
######*/

enum
{
    SPELL_ATTRACT_GHOUL                     = 52037,                // script target on npc 28565
    SPELL_GHOUL_KILL_CREDIT                 = 52030,
    // SPELL_GHOUL_KILL_CREDIT_EFFECT       = 52038,                // triggers 52039; purpose unk - same effect as 52030 but with different target

    NPC_DECAYING_GHOUL                      = 28565,
};

struct npc_ghoul_feeding_bunnyAI : public ScriptedAI
{
    npc_ghoul_feeding_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAttractTimer;

    void Reset() override
    {
        m_uiAttractTimer = 1000;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_DECAYING_GHOUL)
        {
            // Give kill credit to the summoner player
            if (m_creature->IsTemporarySummon())
            {
                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                    DoCastSpellIfCan(pSummoner, SPELL_GHOUL_KILL_CREDIT, CAST_TRIGGERED);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAttractTimer)
        {
            if (m_uiAttractTimer <= uiDiff)
            {
                // try to target a nearby ghoul
                if (DoCastSpellIfCan(m_creature, SPELL_ATTRACT_GHOUL) == CAST_OK)
                    m_uiAttractTimer = 0;
                else
                    m_uiAttractTimer = 5000;
            }
            else
                m_uiAttractTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_ghoul_feeding_bunny(Creature* pCreature)
{
    return new npc_ghoul_feeding_bunnyAI(pCreature);
}

/*######
## npc_decaying_ghoul
######*/

enum
{
    SPELL_BIRTH                             = 26047,
    SPELL_FLESH_ROT                         = 28913,

    NPC_GHOUL_FEEDING_BUNNY                 = 28591,
};

struct npc_decaying_ghoulAI : public ScriptedAI
{
    npc_decaying_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bSpawnAnim = false;
        Reset();
    }

    uint32 m_uiFleshRotTimer;
    bool m_bSpawnAnim;
    ObjectGuid m_feedingBunnyGuid;

    void Reset() override
    {
        m_uiFleshRotTimer = urand(1000, 3000);
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(m_creature, SPELL_BIRTH);
        m_creature->HandleEmote(EMOTE_STATE_NONE);
        m_feedingBunnyGuid.Clear();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // handle the animation and despawn
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->HandleEmote(EMOTE_STATE_EAT_NO_SHEATHE);
        m_creature->ForcedDespawn(10000);

        // send AI event for the quest credit
        if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_feedingBunnyGuid))
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pBunny);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_GHOUL_FEEDING_BUNNY)
        {
            // check if the ghoul has already a feeding bunny set
            if (m_feedingBunnyGuid)
                return;

            // move the ghoul to the feeding target
            float fX, fY, fZ;
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->Clear();
            pInvoker->GetContactPoint(m_creature, fX, fY, fZ);

            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            m_feedingBunnyGuid = pInvoker->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // cast birth animation
        if (!m_bSpawnAnim)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BIRTH) == CAST_OK)
                m_bSpawnAnim = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFleshRotTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLESH_ROT) == CAST_OK)
                m_uiFleshRotTimer = urand(7000, 15000);
        }
        else
            m_uiFleshRotTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_decaying_ghoul(Creature* pCreature)
{
    return new npc_decaying_ghoulAI(pCreature);
}

bool EffectDummyCreature_npc_decaying_ghoul(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_ATTRACT_GHOUL && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_DECAYING_GHOUL)
    {
        if (pCaster->GetEntry() != NPC_GHOUL_FEEDING_BUNNY)
            return true;

        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

struct GymersBuddy : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // this is required because effect 1 happens before effect 0 and at time of visibility update gymer needs to be in second phase in order for control not to malfunction
        if (target)
            target->SetPhaseMask(256, false);
    }
};

void AddSC_zuldrak()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_gurgthock";
    pNewScript->GetAI = &GetAI_npc_gurgthock;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_gurgthock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ghoul_feeding_bunny";
    pNewScript->GetAI = &GetAI_npc_ghoul_feeding_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_decaying_ghoul";
    pNewScript->GetAI = &GetAI_npc_decaying_ghoul;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_decaying_ghoul;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GymersBuddy>("spell_gymers_buddy");
}
