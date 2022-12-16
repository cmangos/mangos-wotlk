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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

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

UnitAI* GetAI_npc_floating_spirit(Creature* pCreature)
{
    return new npc_floating_spiritAI(pCreature);
}

/*######
## npc_restless_frostborn
######*/

bool EffectDummyCreature_npc_restless_frostborn(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_BLOW_HODIRS_HORN && uiEffIndex == EFFECT_INDEX_0 && !pCreatureTarget->IsAlive() && pCaster->GetTypeId() == TYPEID_PLAYER)
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
        pCaster->CastSpell(pCaster, uiSpawnSpell, TRIGGERED_OLD_TRIGGERED);
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

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(true, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            SetEscortPaused(true);

            // set alternative waypoints if required
            if (m_creature->GetPositionX() > 6650.0f)
                SetCurrentWaypoint(8);
            else if (m_creature->GetPositionX() > 6635.0f)
                SetCurrentWaypoint(36);

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
            case 34:
                DoScriptText(SAY_MINER_COMPLETE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_BITTER_DEPARTURE, m_creature);
                    m_creature->SetFacingToObject(pPlayer);
                }
                break;
            case 35:
                m_creature->ForcedDespawn();
                break;
            case 47:
                // merge with the other wp path
                SetEscortPaused(true);
                SetCurrentWaypoint(14);
                SetEscortPaused(false);
                break;
        }
    }
};

UnitAI* GetAI_npc_injured_miner(Creature* pCreature)
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

// 54997 - Cast Net
struct CastNetStormforgedPursuer : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const
    {
        if (ObjectGuid target = spell->m_targets.getUnitTargetGuid()) // can be cast only on this target
            if (target.GetEntry() != 29696) // Stormforged Pursuer
                return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## npc_ethereal_frostworg
######*/

enum
{    // misc
    SPELL_SUMMON_INFILTRATOR                 = 56325,
    NPC_STORMFORGED_INFILTRATOR              = 30222,

    FROSTWOLF_MOVE_NORMAL = 1,
    FROSTWOLF_MOVE_FINAL = 2,

};

enum FrostwolfActions
{
    FROSTWOLF_CHANGE_DIRECTION,
    FROSTWOLF_RESUME_SEARCH,
    FROSTWOLF_FIND_INFILTRATOR,
    FROSTWOLF_SUMMON_INFILTRATOR,
};

static const uint32 ethereal_frostworg_emotes[4][2] =
{
    {30909, 30910},
    {30911, 30912},
    {30913, 30914},
    {30915, 30915}
};

struct npc_ethereal_frostworgAI : public ScriptedAI
{
    npc_ethereal_frostworgAI(Creature* creature) : ScriptedAI(creature)
    {
        AddCustomAction(FROSTWOLF_FIND_INFILTRATOR, 45s, [&]()
        {
            DisableTimer(FROSTWOLF_CHANGE_DIRECTION);
            DisableTimer(FROSTWOLF_RESUME_SEARCH);
            MoveToNewPoint(FROSTWOLF_MOVE_FINAL);
        }, TIMER_COMBAT_OOC);
        AddCustomAction(FROSTWOLF_CHANGE_DIRECTION, 15s, [&]()
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            ResetTimer(FROSTWOLF_RESUME_SEARCH, 3s);
            ResetTimer(FROSTWOLF_CHANGE_DIRECTION, 15s);
        }, TIMER_COMBAT_OOC);
        AddCustomAction(FROSTWOLF_RESUME_SEARCH, 0s, [&]()
        {
            switch (m_emoteCounter)
            {
                case 0:
                case 1:
                case 2:
                {
                    if (!m_creature->IsInCombat())
                        DoBroadcastText(ethereal_frostworg_emotes[m_emoteCounter][urand(0,1)], m_creature);
                    break;
                }
                case 3: break;
                default: m_creature->ForcedDespawn(); break;
            }
            ++m_emoteCounter;
            
            MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
        }, TIMER_COMBAT_OOC);
    }

    uint32 m_emoteCounter = 0;
    bool m_summoned = false;

    void MoveToNewPoint(uint32 type)
    {
        float dist = type == 1 ? 50.f : 20.f;
        float x, y, z = 0.f;
        while (abs(z - m_creature->GetPositionZ()) > 5)
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), dist, x, y, z, 10.f);
        m_creature->GetMotionMaster()->MovePoint(type, Position(x, y, z), FORCED_MOVEMENT_RUN);
    }

    void Reset() override
    {
        MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STORMFORGED_INFILTRATOR)
            m_creature->ForcedDespawn();
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (urand(0,2))
        {
            case 0: DoBroadcastText(30916, summoned); break;
            case 1: DoBroadcastText(30917, summoned); break;
            case 2: DoBroadcastText(30918, summoned); break;
        }
    }

    void MovementInform(uint32 movementType, uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case FROSTWOLF_MOVE_NORMAL:
            {
                MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
                break;
            }
            case FROSTWOLF_MOVE_FINAL:
            {
                if (m_summoned)
                    return;
                m_summoned = true;
                m_creature->GetMotionMaster()->Clear(true, true);
                AddCustomAction(FROSTWOLF_SUMMON_INFILTRATOR, 2s, [&]() { DoCastSpellIfCan(nullptr, SPELL_SUMMON_INFILTRATOR); });
                DoBroadcastText(ethereal_frostworg_emotes[3][0], m_creature);
                break;
            }
            default: break;
        }
    }
};

void AddSC_storm_peaks()
{
    Script* pNewScript = new Script;
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

    pNewScript = new Script;
    pNewScript->Name = "npc_ethereal_frostworg";
    pNewScript->GetAI = &GetNewAIInstance<npc_ethereal_frostworgAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<CastNetStormforgedPursuer>("spell_cast_net_stormforged_pursuer");
}
