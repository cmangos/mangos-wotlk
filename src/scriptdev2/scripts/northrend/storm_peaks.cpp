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
SDName: Storm_Peaks
SD%Complete: 100
SDComment: Quest support: 12832, 12977.
SDCategory: Storm Peaks
EndScriptData */

/* ContentData
npc_floating_spirit
npc_restless_frostborn
npc_injured_miner
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_floating_spirit
######*/

enum
{
    SPELL_BLOW_HODIRS_HORN              = 55983,
    SPELL_SUMMON_FROST_GIANG_SPIRIT     = 55986,
    SPELL_SUMMON_FROST_WARRIOR_SPIRIT   = 55991,
    SPELL_SUMMON_FROST_GHOST_SPIRIT     = 55992,

    NPC_FROST_GIANT_GHOST_KC            = 30138,
    NPC_FROST_DWARF_GHOST_KC            = 30139,

    NPC_NIFFELEM_FOREFATHER             = 29974,
    NPC_FROSTBORN_WARRIOR               = 30135,
    NPC_FROSTBORN_GHOST                 = 30144,
};

struct npc_floating_spiritAI : public ScriptedAI
{
    npc_floating_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        // Simple animation for the floating spirit
        m_creature->SetLevitate(true);
        m_creature->ForcedDespawn(5000);

        m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 50.0f);
    }
};

CreatureAI* GetAI_npc_floating_spirit(Creature* pCreature)
{
    return new npc_floating_spiritAI(pCreature);
}

/*######
## npc_restless_frostborn
######*/

bool EffectDummyCreature_npc_restless_frostborn(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_BLOW_HODIRS_HORN && uiEffIndex == EFFECT_INDEX_0 && !pCreatureTarget->isAlive() && pCaster->GetTypeId() == TYPEID_PLAYER)
    {
        uint32 uiCredit = 0;
        uint32 uiSpawnSpell = 0;
        switch (pCreatureTarget->GetEntry())
        {
            case NPC_NIFFELEM_FOREFATHER:
                uiCredit = NPC_FROST_GIANT_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_GIANG_SPIRIT;
                break;
            case NPC_FROSTBORN_WARRIOR:
                uiCredit = NPC_FROST_DWARF_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_WARRIOR_SPIRIT;
                break;
            case NPC_FROSTBORN_GHOST:
                uiCredit = NPC_FROST_DWARF_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_GHOST_SPIRIT;
                break;
        }

        // spawn the spirit and give the credit; spirit animation is handled by the script above
        pCaster->CastSpell(pCaster, uiSpawnSpell, true);
        ((Player*)pCaster)->KilledMonsterCredit(uiCredit);
        return true;
    }

    return false;
}

/*######
## npc_injured_miner
######*/

enum
{
    // yells
    SAY_MINER_READY                     = -1001051,
    SAY_MINER_COMPLETE                  = -1001052,

    // gossip
    GOSSIP_ITEM_ID_READY                = -3000112,
    TEXT_ID_POISONED                    = 13650,
    TEXT_ID_READY                       = 13651,

    // misc
    SPELL_FEIGN_DEATH                   = 51329,
    QUEST_ID_BITTER_DEPARTURE           = 12832,
};

struct npc_injured_minerAI : public npc_escortAI
{
    npc_injured_minerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(true, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            SetEscortPaused(true);

            // set alternative waypoints if required
            if (m_creature->GetPositionX() > 6650.0f)
                SetCurrentWaypoint(7);
            else if (m_creature->GetPositionX() > 6635.0f)
                SetCurrentWaypoint(35);

            DoScriptText(SAY_MINER_READY, m_creature);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
        else if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            SetEscortPaused(false);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 33:
                DoScriptText(SAY_MINER_COMPLETE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->GroupEventHappens(QUEST_ID_BITTER_DEPARTURE, m_creature);
                    m_creature->SetFacingToObject(pPlayer);
                }
                break;
            case 34:
                m_creature->ForcedDespawn();
                break;
            case 46:
                // merge with the other wp path
                SetEscortPaused(true);
                SetCurrentWaypoint(13);
                SetEscortPaused(false);
                break;
        }
    }
};

CreatureAI* GetAI_npc_injured_miner(Creature* pCreature)
{
    return new npc_injured_minerAI(pCreature);
}

bool GossipHello_npc_injured_miner(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (!pCreature->HasAura(SPELL_FEIGN_DEATH))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY, pCreature->GetObjectGuid());
        return true;
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_POISONED, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_injured_miner(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature);
    }

    return true;
}

bool QuestAccept_npc_injured_miner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_BITTER_DEPARTURE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

void AddSC_storm_peaks()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_floating_spirit";
    pNewScript->GetAI = &GetAI_npc_floating_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_restless_frostborn";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_restless_frostborn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_miner";
    pNewScript->GetAI = &GetAI_npc_injured_miner;
    pNewScript->pGossipHello = &GossipHello_npc_injured_miner;
    pNewScript->pGossipSelect = &GossipSelect_npc_injured_miner;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_injured_miner;
    pNewScript->RegisterSelf();
}
