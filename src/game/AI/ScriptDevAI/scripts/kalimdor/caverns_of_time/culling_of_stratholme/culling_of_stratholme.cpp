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
SDName: culling_of_stratholme
SD%Complete: 80%
SDComment: Zombies spawns partially implemented
SDCategory: Culling of Stratholme
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "culling_of_stratholme.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "Spells/SpellAuras.h"

/* *************
** npc_chromie (gossip, quest-accept)
************* */

enum
{
    QUEST_DISPELLING_ILLUSIONS  = 13149,
    QUEST_A_ROYAL_ESCORT        = 13151,

    ITEM_ARCANE_DISRUPTOR       = 37888,

    GOSSIP_ITEM_ENTRANCE_1      = -3595000,
    GOSSIP_ITEM_ENTRANCE_2      = -3595001,
    GOSSIP_ITEM_ENTRANCE_3      = -3595002,

    TEXT_ID_ENTRANCE_1          = 12992,
    TEXT_ID_ENTRANCE_2          = 12993,
    TEXT_ID_ENTRANCE_3          = 12994,
    TEXT_ID_ENTRANCE_4          = 12995,

    GOSSIP_ITEM_INN_1           = -3595003,
    GOSSIP_ITEM_INN_2           = -3595004,
    GOSSIP_ITEM_INN_3           = -3595005,
    GOSSIP_ITEM_INN_SKIP        = -3595006,                 // used to skip the intro; requires research
    GOSSIP_ITEM_INN_TELEPORT    = -3595007,                 // teleport to stratholme - used after the main event has started

    TEXT_ID_INN_1               = 12939,
    TEXT_ID_INN_2               = 12949,
    TEXT_ID_INN_3               = 12950,
    TEXT_ID_INN_4               = 12952,
    TEXT_ID_INN_TELEPORT        = 13470,

    SPELL_TELEPORT_COT_P4       = 53435,                    // triggers 53436
};

bool GossipHello_npc_chromie(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_culling_of_stratholme* m_pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
    {
        switch (pCreature->GetEntry())
        {
            case NPC_CHROMIE_INN:
                if (m_pInstance->GetData(TYPE_GRAIN_EVENT) != DONE)
                {
                    if (pPlayer->GetQuestRewardStatus(QUEST_DISPELLING_ILLUSIONS) && !pPlayer->HasItemCount(ITEM_ARCANE_DISRUPTOR, 1))
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                }
                // intro skip option is available since 3.3.x
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_SKIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_1, pCreature->GetObjectGuid());
                break;
            case NPC_CHROMIE_ENTRANCE:
                if (m_pInstance->GetData(TYPE_GRAIN_EVENT) == DONE && m_pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED && pPlayer->GetQuestRewardStatus(QUEST_A_ROYAL_ESCORT))
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_1, pCreature->GetObjectGuid());
                break;
        }
    }
    return true;
}

bool GossipSelect_npc_chromie(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHROMIE_INN:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_2, pCreature->GetObjectGuid());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_3, pCreature->GetObjectGuid());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_4, pCreature->GetObjectGuid());
                    if (!pPlayer->HasItemCount(ITEM_ARCANE_DISRUPTOR, 1))
                    {
                        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_ARCANE_DISRUPTOR, 1))
                        {
                            pPlayer->SendNewItem(pItem, 1, true, false);
                            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
                            {
                                if (pInstance->GetData(TYPE_GRAIN_EVENT) == NOT_STARTED)
                                    pInstance->SetData(TYPE_GRAIN_EVENT, SPECIAL);
                            }
                        }
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF+4:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_TELEPORT, pCreature->GetObjectGuid());
                    break;
                case GOSSIP_ACTION_INFO_DEF+5:
                    pCreature->CastSpell(pPlayer, SPELL_TELEPORT_COT_P4, TRIGGERED_OLD_TRIGGERED);
                    if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
                    {
                        // only skip intro if not already started;
                        if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED && pInstance->GetData(TYPE_GRAIN_EVENT) == NOT_STARTED)
                        {
                            pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, DONE);
                            pInstance->SetData(TYPE_GRAIN_EVENT, DONE);

                            // spawn Arthas and Chromie
                            pInstance->DoSpawnChromieIfNeeded(pPlayer);
                            pInstance->DoSpawnArthasIfNeeded(pPlayer);
                        }
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
            }
            break;
        case NPC_CHROMIE_ENTRANCE:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_2, pCreature->GetObjectGuid());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_3, pCreature->GetObjectGuid());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_4, pCreature->GetObjectGuid());
                    if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
                    {
                        if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED)
                        {
                            pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, IN_PROGRESS);
                            pInstance->DoSpawnArthasIfNeeded(pPlayer);
                        }
                    }
                    break;
            }
            break;
    }
    return true;
}

bool QuestAccept_npc_chromie(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    switch (pQuest->GetQuestId())
    {
        case QUEST_DISPELLING_ILLUSIONS:
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
            {
                if (pInstance->GetData(TYPE_GRAIN_EVENT) == NOT_STARTED)
                    pInstance->SetData(TYPE_GRAIN_EVENT, SPECIAL);
            }
            break;
        case QUEST_A_ROYAL_ESCORT:
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
            {
                if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED)
                {
                    pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, IN_PROGRESS);
                    pInstance->DoSpawnArthasIfNeeded(pPlayer);
                }
            }
            break;
    }
    return true;
}

/* *************
** npc_crates_bunny (spell aura effect dummy)
************* */

enum
{
    SAY_SOLDIERS_REPORT         = -1595000,

    SPELL_ARCANE_DISRUPTION     = 49590,
    SPELL_CRATES_KILL_CREDIT    = 58109,
};

bool EffectAuraDummy_spell_aura_dummy_npc_crates_dummy(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_ARCANE_DISRUPTION && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            if (pTarget->GetEntry() != NPC_GRAIN_CRATE_HELPER)
                return true;

            CreatureList lCrateBunnyList;
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pTarget->GetInstanceData())
            {
                pInstance->GetCratesBunnyOrderedList(lCrateBunnyList);
                uint8 i = 0;
                for (CreatureList::const_iterator itr = lCrateBunnyList.begin(); itr != lCrateBunnyList.end(); ++itr)
                {
                    ++i;
                    if (*itr == pTarget)
                    {
                        // check if the event can proceed
                        if (!pInstance->CanGrainEventProgress(pTarget))
                            return true;

                        break;
                    }
                }

                switch (i)
                {
                    case 1:
                        // Start NPC_ROGER_OWENS Event
                        if (Creature* pRoger = pInstance->GetSingleCreatureFromStorage(NPC_ROGER_OWENS))
                        {
                            pRoger->SetStandState(UNIT_STAND_STATE_STAND);
                            pRoger->GetMotionMaster()->MoveWaypoint();
                        }
                        break;
                    case 2:
                        // Start NPC_SERGEANT_MORIGAN  Event
                        if (Creature* pMorigan = pInstance->GetSingleCreatureFromStorage(NPC_SERGEANT_MORIGAN))
                            pMorigan->GetMotionMaster()->MoveWaypoint();
                        break;
                    case 3:
                        // Start NPC_JENA_ANDERSON Event
                        if (Creature* pJena = pInstance->GetSingleCreatureFromStorage(NPC_JENA_ANDERSON))
                            pJena->GetMotionMaster()->MoveWaypoint();
                        break;
                    case 4:
                        // Start NPC_MALCOM_MOORE Event
                        pTarget->SummonCreature(NPC_MALCOM_MOORE, 1605.452f, 804.9279f, 122.961f, 5.19f, TEMPSPAWN_DEAD_DESPAWN, 0);
                        break;
                    case 5:
                        // Start NPC_BARTLEBY_BATTSON Event
                        if (Creature* pBartleby = pInstance->GetSingleCreatureFromStorage(NPC_BARTLEBY_BATTSON))
                            pBartleby->GetMotionMaster()->MoveWaypoint();
                        break;
                }

                // Finished event, give killcredit
                if (pInstance->GetData(TYPE_GRAIN_EVENT) == DONE)
                {
                    pTarget->CastSpell(pTarget, SPELL_CRATES_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
                    pInstance->DoOrSimulateScriptTextForThisInstance(SAY_SOLDIERS_REPORT, NPC_LORDAERON_CRIER);
                }

                // despawn the GO visuals and spanw the plague crate
                if (GameObject* pCrate = GetClosestGameObjectWithEntry(pTarget, GO_SUSPICIOUS_GRAIN_CRATE, 5.0f))
                    pCrate->SetLootState(GO_JUST_DEACTIVATED);
                if (GameObject* pHighlight = GetClosestGameObjectWithEntry(pTarget, GO_CRATE_HIGHLIGHT, 5.0f))
                    pHighlight->SetLootState(GO_JUST_DEACTIVATED);
                if (GameObject* pCrate = GetClosestGameObjectWithEntry(pTarget, GO_PLAGUE_GRAIN_CRATE, 5.0f))
                {
                    pCrate->SetRespawnTime(6 * HOUR * IN_MILLISECONDS);
                    pCrate->Refresh();
                }
            }
        }
    }
    return true;
}

/* *************
** npc_arthas
************* */

enum
{
    // texts
    SAY_ARTHAS_FOLLOW           = -1595009,
    SAY_CITIZEN_ARRIVED         = -1595010,
    SAY_ARTHAS_BEFORE_PLAGUE    = -1595011,
    SAY_ARTHAS_SORCERY          = -1595012,
    SAY_CITIZEN_NO_UNDERSTAD    = -1595013,
    SAY_ARTHAS_MORE_SCOURGE     = -1595014,
    SAY_ARTHAS_MORE_SORCERY     = -1595015,
    SAY_ARTHAS_MOVE_ON          = -1595016,
    SAY_ARTHAS_WATCH_BACK       = -1595017,
    SAY_ARTHAS_NOT_EASY         = -1595018,
    SAY_ARTHAS_PERSISTENT       = -1595019,
    SAY_ARTHAS_WHAT_ELSE        = -1595020,
    SAY_EPOCH_DARKNESS          = -1595021,
    SAY_ARTHAS_DO_WHAT_MUST     = -1595022,

    SAY_ARTHAS_QUICK_PATH       = -1595023,
    SAY_ARTHAS_TAKE_A_MOMENT    = -1595024,
    SAY_ARTHAS_PASSAGE          = -1595025,

    SAY_ARTHAS_MOVE_QUICKLY     = -1595026,
    SAY_ARTHAS_REST             = -1595027,
    SAY_ARTHAS_REST_COMPLETE    = -1595028,
    SAY_ARTHAS_CRUSADER_SQUARE  = -1595029,

    SAY_ARTHAS_JUSTICE_DONE     = -1595030,
    SAY_ARTHAS_MALGANIS         = -1595031,
    SAY_MALGANIS_JOURNEY        = -1595032,
    SAY_ARTHAS_HUNT_MALGANIS    = -1595033,             // play music 14951
    SAY_ARTHAS_EVENT_COMPLETE   = -1595034,

    SAY_ARTHAS_HALF_HP          = -1595035,
    SAY_ARTHAS_LOW_HP           = -1595036,
    SAY_ARTHAS_SLAY_1           = -1595037,
    SAY_ARTHAS_SLAY_2           = -1595038,
    SAY_ARTHAS_SLAY_3           = -1595039,
    SAY_ARTHAS_DEATH            = -1595040,

    MUSIC_ID_EVENT_COMPLETE     = 14951,

    // gossips
    GOSSIP_ITEM_CITY_GATES      = -3595008,
    GOSSIP_ITEM_TOWN_HALL_1     = -3595009,
    GOSSIP_ITEM_TOWN_HALL_2     = -3595010,
    GOSSIP_ITEM_EPOCH           = -3595011,
    GOSSIP_ITEM_ESCORT          = -3595012,
    GOSSIP_ITEM_DREADLORD       = -3595013,

    TEXT_ID_CITY_GATES          = 13076,
    TEXT_ID_TOWN_HALL_1         = 13125,
    TEXT_ID_TOWN_HALL_2         = 13126,
    TEXT_ID_EPOCH               = 13177,
    TEXT_ID_ESCORT              = 13179,
    TEXT_ID_DREADLORD           = 13287,

    // spells
    SPELL_HOLY_LIGHT            = 52444,
    SPELL_EXORCISM              = 52445,
    SPELL_EXORCISM_H            = 58822,
    SPELL_DEVOTION_AURA         = 52442,
    SPELL_CRUSADER_STRIKE       = 50773,                // dummy used on citizens

    SPELL_MALGANIS_KILL_CREDIT  = 58124,
    SPELL_MALGANIS_ACHIEVEMENT  = 58630,                // server side spell

    SPELL_TRANSFORM             = 33133,
    SPELL_SHADOWSTEP_COSMETIC   = 51908,

    // FACTION_ARTHAS_1         = 2076,                 // default faction
    FACTION_ARTHAS_2            = 2077,                 // burning city
    // FACTION_ARTHAS_3         = 2079,                 // use unk

    POINT_ID_INTRO_COMPLETE     = 31,                   // intro completion WP - handled by dbscript
    POINT_ID_ESCORT_CITY        = 47,                   // escort point before burning city
    POINT_ID_MARKET_ROW         = 70,                   // escort point before Malganis
};

static const DialogueEntry aArthasDialogue[] =
{
    {NPC_TOWNHALL_CITIZEN,      0,              2000},
    {TEXT_ID_TOWN_HALL_1,       0,              0},
    // transform citizens
    {SPELL_CRUSADER_STRIKE,     0,              2000},
    {NPC_TOWNHALL_RESIDENT,     0,              1000},
    {SAY_ARTHAS_SORCERY,        NPC_ARTHAS,     3000},
    {TEXT_ID_TOWN_HALL_2,       0,              1000},
    {NPC_INFINITE_HUNTER,       0,              1000},
    {NPC_INFINITE_AGENT,        0,              1000},
    {NPC_INFINITE_ADVERSARY,    0,              0},
    // start Epoch fight
    {SAY_ARTHAS_WHAT_ELSE,      NPC_ARTHAS,     5000},
    {SAY_EPOCH_DARKNESS,        NPC_LORD_EPOCH, 13000},
    {SAY_ARTHAS_DO_WHAT_MUST,   NPC_ARTHAS,     5000},
    {NPC_ARTHAS,                0,              2000},
    {NPC_LORD_EPOCH,            0,              0},
    // open bookcase door
    {SAY_ARTHAS_TAKE_A_MOMENT,  NPC_ARTHAS,     2000},
    {GO_DOOR_BOOKCASE,          0,              0},
    // reach crusader square
    {SAY_ARTHAS_CRUSADER_SQUARE, NPC_ARTHAS,    10000},
    {SPELL_TRANSFORM,           0,              0},
    // start Malganis fight
    {SAY_ARTHAS_MALGANIS,       NPC_ARTHAS,     5000},
    {NPC_MALGANIS,              0,              0},
    // Malganis epilog
    {SAY_MALGANIS_JOURNEY,      NPC_MALGANIS,   18000},
    {SAY_ARTHAS_HUNT_MALGANIS,  NPC_ARTHAS,     5000},
    {TEXT_ID_DREADLORD,         0,              2000},
    {SAY_ARTHAS_EVENT_COMPLETE, NPC_ARTHAS,     0},
    {0, 0, 0},
};

struct npc_arthasAI : public npc_escortAI, private DialogueHelper
{
    npc_arthasAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aArthasDialogue)
    {
        m_pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_culling_of_stratholme* m_pInstance;

    bool m_bIsRegularMode;

    bool m_bYell50Pct;
    bool m_bYell20Pct;

    uint32 m_uiHolyLightTimer;
    uint32 m_uiExorcismTimer;

    ObjectGuid m_firstCitizenGuid;
    ObjectGuid m_secondCitizenGuid;
    ObjectGuid m_thirdCitizenGuid;

    GuidList m_lSummonedGuidsList;

    void Reset() override
    {
        m_uiExorcismTimer   = urand(5000, 10000);
        m_uiHolyLightTimer  = urand(5000, 10000);
        m_bYell50Pct        = false;
        m_bYell20Pct        = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_DEVOTION_AURA);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_ARTHAS_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_ARTHAS_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_ARTHAS_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_ARTHAS_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARTHAS_ESCORT_EVENT, FAIL);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 uiMiscValue) override
    {
        // on Malganis encounter finished -> evade
        if (pSender->GetEntry() == NPC_MALGANIS && eventType == AI_EVENT_CUSTOM_EVENTAI_B)
        {
            EnterEvadeMode();
            SetEscortPaused(false);
            return;
        }

        if (pInvoker->GetTypeId() != TYPEID_PLAYER)
            return;

        if (eventType == AI_EVENT_START_ESCORT)
        {
            DoScriptText(SAY_ARTHAS_FOLLOW, m_creature);
            Start(true, (Player*)pInvoker);
        }
        else if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(SAY_ARTHAS_QUICK_PATH, m_creature);
            SetEscortPaused(false);
        }
        else if (eventType == AI_EVENT_START_ESCORT_B)
        {
            // start and set WP
            Start(true, (Player*)pInvoker);
            SetEscortPaused(true);
            SetCurrentWaypoint(POINT_ID_ESCORT_CITY);
            SetEscortPaused(false);

            DoScriptText(SAY_ARTHAS_MOVE_QUICKLY, m_creature);
            m_creature->SetFactionTemporary(FACTION_ARTHAS_2, TEMPFACTION_RESTORE_REACH_HOME);

            if (m_pInstance)
                m_pInstance->DoSpawnBurningCityUndead(m_creature);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // resume escort or start on point if start from reset
            if (HasEscortState(STATE_ESCORT_PAUSED))
                SetEscortPaused(false);
            else
            {
                Start(true, (Player*)pInvoker);
                SetEscortPaused(true);
                SetCurrentWaypoint(POINT_ID_MARKET_ROW);
                SetEscortPaused(false);
            }

            DoScriptText(SAY_ARTHAS_JUSTICE_DONE, m_creature);
            m_creature->SetFactionTemporary(FACTION_ARTHAS_2, TEMPFACTION_RESTORE_REACH_HOME);

            // spawn Malganis if required
            if (m_pInstance && !m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS, true))
                m_creature->SummonCreature(NPC_MALGANIS, 2296.862F, 1501.015F, 128.445F, 5.13f, TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Arthas latest reached waypoint %u", m_creature->GetMotionMaster()->getLastReachedWaypoint());
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType == WAYPOINT_MOTION_TYPE)
        {
            // set the intro event as done and start the undead waves
            if (uiPointId == POINT_ID_INTRO_COMPLETE && m_pInstance)
                m_pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, DONE);
        }
        else
            npc_escortAI::MovementInform(uiType, uiPointId);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_INFINITE_ADVERSARY:
            case NPC_INFINITE_AGENT:
            case NPC_INFINITE_HUNTER:
                pSummoned->AI()->AttackStart(m_creature);
            // no break;
            case NPC_TOWNHALL_CITIZEN:
            case NPC_TOWNHALL_RESIDENT:
                m_lSummonedGuidsList.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_LORD_EPOCH:
                pSummoned->GetMotionMaster()->MovePoint(0, 2450.874f, 1113.122f, 149.008f);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_INFINITE_ADVERSARY:
            case NPC_INFINITE_AGENT:
            case NPC_INFINITE_HUNTER:
                m_lSummonedGuidsList.remove(pSummoned->GetObjectGuid());

                if (m_lSummonedGuidsList.empty())
                    SetEscortPaused(false);
                break;
            case NPC_LORD_EPOCH:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            // spawn citizens - ground floor
            case 1:
                if (Creature* pCitizen = m_creature->SummonCreature(NPC_TOWNHALL_CITIZEN, 2401.265f, 1202.789f, 134.103f, 1.466f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000))
                    m_firstCitizenGuid = pCitizen->GetObjectGuid();
                if (Creature* pCitizen = m_creature->SummonCreature(NPC_TOWNHALL_RESIDENT, 2402.654f, 1205.786f, 134.122f, 2.89f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000))
                    m_secondCitizenGuid = pCitizen->GetObjectGuid();
                if (Creature* pCitizen = m_creature->SummonCreature(NPC_TOWNHALL_CITIZEN, 2398.715f, 1207.334f, 134.122f, 5.27f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000))
                    m_thirdCitizenGuid = pCitizen->GetObjectGuid();
                break;
            case 3:
                StartNextDialogueText(NPC_TOWNHALL_CITIZEN);
                break;
            case 4:
                DoScriptText(SAY_ARTHAS_BEFORE_PLAGUE, m_creature);
                SetRun(false);
                break;
            case 5:
                SetEscortPaused(true);
                StartNextDialogueText(SPELL_CRUSADER_STRIKE);
                break;
            case 6:
                m_creature->SetFacingTo(0.30f);
                break;
            case 7:
                DoScriptText(SAY_ARTHAS_MORE_SCOURGE, m_creature);
                SetRun();
                break;

            // spawn second wave - on the first floor
            case 13:
                DoScriptText(SAY_ARTHAS_MORE_SORCERY, m_creature);
                SetRun(false);
                m_creature->SummonCreature(NPC_TIME_RIFT, 2433.357f, 1192.168f, 148.159f, 3.00f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                break;
            case 14:
                m_creature->SummonCreature(NPC_INFINITE_AGENT, 2433.041f, 1191.158f, 148.128f, 4.99f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_HUNTER, 2433.176f, 1193.429f, 148.114f, 1.21f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_AGENT, 2433.396f, 1192.912f, 148.123f, 0.04f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_ADVERSARY, 2433.626f, 1192.069f, 148.117f, 5.48f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                SetEscortPaused(true);
                break;
            case 15:
                m_creature->SetFacingTo(2.94f);
                break;
            case 16:
                DoScriptText(SAY_ARTHAS_MOVE_ON, m_creature);
                SetRun();
                break;

            // spawn third wave - ambush
            case 21:
                DoScriptText(SAY_ARTHAS_WATCH_BACK, m_creature);

                m_creature->SummonCreature(NPC_INFINITE_HUNTER, 2404.375f, 1179.395f, 148.138f, 5.14f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_AGENT, 2403.785f, 1179.004f, 148.138f, 4.58f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_HUNTER, 2413.933f, 1136.93f, 148.111f, 2.40f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_ADVERSARY, 2414.964f, 1136.857f, 148.088f, 0.90f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_TIME_RIFT, 2404.311f, 1178.306f, 148.158f, 1.60f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_TIME_RIFT, 2414.041f, 1136.068f, 148.159f, 2.23f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                SetEscortPaused(true);
                break;
            case 22:
                m_creature->SetFacingTo(1.85f);
                break;
            case 23:
                DoScriptText(SAY_ARTHAS_NOT_EASY, m_creature);
                break;

            // spawn forth wave - main hall
            case 26:
                DoScriptText(SAY_ARTHAS_PERSISTENT, m_creature);

                m_creature->SummonCreature(NPC_INFINITE_HUNTER, 2431.417f, 1105.167f, 148.075f, 1.26f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_AGENT, 2438.808f, 1113.769f, 148.075f, 3.11f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_ADVERSARY, 2440.917f, 1116.085f, 148.080f, 2.18f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_INFINITE_ADVERSARY, 2428.905f, 1102.929f, 148.125f, 1.97f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_TIME_RIFT, 2429.296f, 1102.007f, 148.159f, 6.21f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                m_creature->SummonCreature(NPC_TIME_RIFT, 2440.057f, 1114.226f, 148.159f, 6.10f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                SetEscortPaused(true);
                break;
            case 27:
                m_creature->SetFacingTo(5.98f);
                break;
            case 28:
                StartNextDialogueText(SAY_ARTHAS_WHAT_ELSE);
                SetEscortPaused(true);
                break;

            // open passage
            case 32:
                StartNextDialogueText(SAY_ARTHAS_TAKE_A_MOMENT);
                break;
            case 33:
                DoScriptText(SAY_ARTHAS_PASSAGE, m_creature);
                break;

            // townhall escort complete
            case 46:
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_ARTHAS_TOWNHALL_EVENT, DONE);
                break;

            // burning stratholme
            case 57:
                DoScriptText(SAY_ARTHAS_REST, m_creature);
                m_creature->SetFacingTo(5.04f);
                break;
            case 58:
                DoScriptText(SAY_ARTHAS_REST_COMPLETE, m_creature);
                break;
            case 69:
                StartNextDialogueText(SAY_ARTHAS_CRUSADER_SQUARE);
                SetEscortPaused(true);
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_ARTHAS_ESCORT_EVENT, DONE);
                break;

            // malganis fight
            case 74:
                StartNextDialogueText(SAY_ARTHAS_MALGANIS);
                SetEscortPaused(true);
                break;

            // event epilog
            case 75:
                StartNextDialogueText(SAY_MALGANIS_JOURNEY);
                break;
            case 76:
                SetRun(false);
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            // townhall entrance dialogue
            case NPC_TOWNHALL_CITIZEN:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_firstCitizenGuid))
                    pCitizen->SetFacingToObject(m_creature);
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_secondCitizenGuid))
                    pCitizen->SetFacingToObject(m_creature);
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                    pCitizen->SetFacingToObject(m_creature);
                break;
            case TEXT_ID_TOWN_HALL_1:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                    DoScriptText(SAY_CITIZEN_ARRIVED, pCitizen);
                break;
            case SPELL_CRUSADER_STRIKE:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                    DoCastSpellIfCan(pCitizen, SPELL_CRUSADER_STRIKE);
                break;
            case NPC_TOWNHALL_RESIDENT:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                    pCitizen->HandleEmote(EMOTE_ONESHOT_LAUGH);
                break;
            case TEXT_ID_TOWN_HALL_2:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                    DoScriptText(SAY_CITIZEN_NO_UNDERSTAD, pCitizen);
                break;
            case NPC_INFINITE_HUNTER:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_firstCitizenGuid))
                {
                    pCitizen->CastSpell(pCitizen, SPELL_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
                    pCitizen->UpdateEntry(NPC_INFINITE_HUNTER);
                }
                break;
            case NPC_INFINITE_AGENT:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_secondCitizenGuid))
                {
                    pCitizen->CastSpell(pCitizen, SPELL_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
                    pCitizen->UpdateEntry(NPC_INFINITE_AGENT);
                }
                break;
            case NPC_INFINITE_ADVERSARY:
                if (Creature* pCitizen = m_creature->GetMap()->GetCreature(m_thirdCitizenGuid))
                {
                    pCitizen->CastSpell(pCitizen, SPELL_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
                    pCitizen->UpdateEntry(NPC_INFINITE_ADVERSARY);
                }
                break;

            // epoch event dialogue
            case SAY_ARTHAS_WHAT_ELSE:
                m_creature->SummonCreature(NPC_LORD_EPOCH, 2456.396f, 1113.969f, 150.032f, 3.15f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_TIME_RIFT_BIG, 2456.058f, 1113.838f, 150.091f, 1.74f, TEMPSPAWN_TIMED_DESPAWN, 5000);
                break;
            case NPC_ARTHAS:
                m_creature->HandleEmote(EMOTE_ONESHOT_POINT_NOSHEATHE);
                break;
            case NPC_LORD_EPOCH:
                if (!m_pInstance)
                    return;

                if (Creature* pEpoch = m_pInstance->GetSingleCreatureFromStorage(NPC_LORD_EPOCH))
                {
                    pEpoch->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    pEpoch->AI()->AttackStart(m_creature);
                }
                break;

            // bookcase passage delay
            case GO_DOOR_BOOKCASE:
                if (m_pInstance)
                    m_pInstance->DoUseDoorOrButton(GO_DOOR_BOOKCASE);
                break;

            // burning city complete delay
            case SPELL_TRANSFORM:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;

            // malganis attack
            case NPC_MALGANIS:
                if (!m_pInstance)
                    return;

                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                {
                    pMalganis->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    pMalganis->AI()->AttackStart(m_creature);
                }
                break;

            // malganis event complete
            case SAY_ARTHAS_HUNT_MALGANIS:
                m_creature->PlayMusic(MUSIC_ID_EVENT_COMPLETE);

                if (!m_pInstance)
                    return;

                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                {
                    pMalganis->CastSpell(pMalganis, SPELL_SHADOWSTEP_COSMETIC, TRIGGERED_OLD_TRIGGERED);
                    pMalganis->CastSpell(pMalganis, SPELL_MALGANIS_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
                    pMalganis->ForcedDespawn(1000);
                }
                break;
            case TEXT_ID_DREADLORD:
                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                break;
            case SAY_ARTHAS_EVENT_COMPLETE:
                m_creature->SetFacingTo(5.13f);
                DoCastSpellIfCan(m_creature, SPELL_MALGANIS_KILL_CREDIT, CAST_TRIGGERED);

                if (m_pInstance)
                    m_pInstance->SetData(TYPE_MALGANIS_EVENT, DONE);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiExorcismTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_EXORCISM : SPELL_EXORCISM_H) == CAST_OK)
                m_uiExorcismTimer = urand(9000, 13000);
        }
        else
            m_uiExorcismTimer -= uiDiff;

        if (m_uiHolyLightTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HOLY_LIGHT) == CAST_OK)
                    m_uiHolyLightTimer = urand(18000, 25000);
            }
        }
        else
            m_uiHolyLightTimer -= uiDiff;

        if (!m_bYell50Pct && m_creature->GetHealthPercent() < 50.0f)
        {
            DoScriptText(SAY_ARTHAS_HALF_HP, m_creature);
            m_bYell50Pct = true;
        }

        if (!m_bYell20Pct && m_creature->GetHealthPercent() < 20.0f)
        {
            DoScriptText(SAY_ARTHAS_LOW_HP, m_creature);
            m_bYell20Pct = true;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_arthas(Creature* pCreature)
{
    return new npc_arthasAI(pCreature);
}

bool GossipHello_npc_arthas(Player* pPlayer, Creature* pCreature)
{
    if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
    {
        // city gate gossip
        if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == IN_PROGRESS)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CITY_GATES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_CITY_GATES, pCreature->GetObjectGuid());
        }
        // town hall entrance gossip
        else if (pInstance->GetData(TYPE_SALRAMM_EVENT) == DONE && pInstance->GetData(TYPE_EPOCH_EVENT) != DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TOWN_HALL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_TOWN_HALL_1, pCreature->GetObjectGuid());
        }
        // town hall epoch gossip
        else if (pInstance->GetData(TYPE_EPOCH_EVENT) == DONE && pInstance->GetData(TYPE_ARTHAS_TOWNHALL_EVENT) != DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_EPOCH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_EPOCH, pCreature->GetObjectGuid());
        }
        // burning city gossip
        else if (pInstance->GetData(TYPE_ARTHAS_TOWNHALL_EVENT) == DONE && pInstance->GetData(TYPE_ARTHAS_ESCORT_EVENT) != DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ESCORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORT, pCreature->GetObjectGuid());
        }
        // crusader square gossip
        else if (pInstance->GetData(TYPE_ARTHAS_ESCORT_EVENT) == DONE && pInstance->GetData(TYPE_MALGANIS_EVENT) != DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DREADLORD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DREADLORD, pCreature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_npc_arthas(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:                      // resume WP movement - rest is handled by DB
            pCreature->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TOWN_HALL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_TOWN_HALL_2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:                      // start initial town hall escort event
            pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF+4:                      // continue escort after Epoch
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF+5:                      // start burning city event
            pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT_B, pPlayer, pCreature);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF+6:                      // start Malganis event
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pPlayer, pCreature);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

/* *************
** npc_spell_dummy_crusader_strike
************* */

bool EffectDummyCreature_npc_spell_dummy_crusader_strike(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_CRUSADER_STRIKE && uiEffIndex == EFFECT_INDEX_0)
    {
        // only apply this for certain citizens
        if (pCreatureTarget->GetEntry() == NPC_STRATHOLME_RESIDENT || pCreatureTarget->GetEntry() == NPC_STRATHOLME_CITIZEN)
            pCreatureTarget->Suicide();
        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

void AddSC_culling_of_stratholme()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_chromie";
    pNewScript->pGossipHello = &GossipHello_npc_chromie;
    pNewScript->pGossipSelect = &GossipSelect_npc_chromie;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_chromie;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_npc_crates_bunny";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_npc_crates_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arthas";
    pNewScript->GetAI = &GetAI_npc_arthas;
    pNewScript->pGossipHello = &GossipHello_npc_arthas;
    pNewScript->pGossipSelect = &GossipSelect_npc_arthas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spell_dummy_crusader_strike";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_spell_dummy_crusader_strike;
    pNewScript->RegisterSelf();
}
