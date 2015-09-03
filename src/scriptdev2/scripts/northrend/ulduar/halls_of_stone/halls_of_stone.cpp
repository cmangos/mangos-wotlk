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
SDName: Halls_of_Stone
SD%Complete: 50%
SDComment: Just base mechanics in script, timers and stuff is very uncertain, event-spells are not working
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"
#include "escort_ai.h"

/* Notes
 * The timers and handling of texts is not confirmed, but should also not be too far off
 * The spells "of the statues" (handled in instance script), need quite much of core support
 */

enum
{
    SAY_KILL_1                          = -1599012,
    SAY_KILL_2                          = -1599013,
    SAY_KILL_3                          = -1599014,
    SAY_LOW_HEALTH                      = -1599015,
    SAY_DEATH                           = -1599016,
    SAY_PLAYER_DEATH_1                  = -1599017,
    SAY_PLAYER_DEATH_2                  = -1599018,
    SAY_PLAYER_DEATH_3                  = -1599019,
    SAY_ESCORT_START                    = -1599020,

    SAY_SPAWN_DWARF                     = -1599021,
    SAY_SPAWN_TROGG                     = -1599022,
    SAY_SPAWN_OOZE                      = -1599023,
    SAY_SPAWN_EARTHEN                   = -1599024,

    SAY_EVENT_INTRO_1                   = -1599025,
    SAY_EVENT_INTRO_2                   = -1599026,
    SAY_EVENT_INTRO_3_ABED              = -1599027,

    SAY_EVENT_A_1                       = -1599028,
    SAY_EVENT_A_2_KADD                  = -1599029,
    SAY_EVENT_A_3                       = -1599030,

    SAY_EVENT_B_1                       = -1599031,
    SAY_EVENT_B_2_MARN                  = -1599032,
    SAY_EVENT_B_3                       = -1599033,

    SAY_EVENT_C_1                       = -1599034,
    SAY_EVENT_C_2_ABED                  = -1599035,
    SAY_EVENT_C_3                       = -1599036,

    SAY_EVENT_D_1                       = -1599037,
    SAY_EVENT_D_2_ABED                  = -1599038,
    SAY_EVENT_D_3                       = -1599039,
    SAY_EVENT_D_4_ABED                  = -1599040,

    SAY_EVENT_END_01                    = -1599041,
    SAY_EVENT_END_02                    = -1599042,
    SAY_EVENT_END_03_ABED               = -1599043,
    SAY_EVENT_END_04                    = -1599044,
    SAY_EVENT_END_05_ABED               = -1599045,
    SAY_EVENT_END_06                    = -1599046,
    SAY_EVENT_END_07_ABED               = -1599047,
    SAY_EVENT_END_08                    = -1599048,
    SAY_EVENT_END_09_KADD               = -1599049,
    SAY_EVENT_END_10                    = -1599050,
    SAY_EVENT_END_11_KADD               = -1599051,
    SAY_EVENT_END_12                    = -1599052,
    SAY_EVENT_END_13_KADD               = -1599053,
    SAY_EVENT_END_14                    = -1599054,
    SAY_EVENT_END_15_MARN               = -1599055,
    SAY_EVENT_END_16                    = -1599056,
    SAY_EVENT_END_17_MARN               = -1599057,
    SAY_EVENT_END_18                    = -1599058,
    SAY_EVENT_END_19_MARN               = -1599059,
    SAY_EVENT_END_20                    = -1599060,
    SAY_EVENT_END_21_ABED               = -1599061,

    SAY_VICTORY_SJONNIR_1               = -1599062,
    SAY_VICTORY_SJONNIR_2               = -1599063,

    SAY_ENTRANCE_MEET                   = -1599064,

    GOSSIP_ITEM_ID_START                = -3599000,
    GOSSIP_ITEM_ID_PROGRESS             = -3599001,

    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101,

    SPELL_SUMMON_PROTECTOR              = 51780,                // all spells are casted by stalker npcs 28130
    SPELL_SUMMON_STORMCALLER            = 51050,
    SPELL_SUMMON_CUSTODIAN              = 51051,

    SPELL_STEALTH                       = 58506,

    NPC_DARK_RUNE_PROTECTOR             = 27983,
    NPC_DARK_RUNE_STORMCALLER           = 27984,
    NPC_IRON_GOLEM_CUSTODIAN            = 27985,

    QUEST_HALLS_OF_STONE                = 13207,
};

/*######
## npc_brann_hos
######*/

struct npc_brann_hosAI : public npc_escortAI
{
    npc_brann_hosAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_halls_of_stone*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_halls_of_stone* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bHasContinued;
    bool m_bIsBattle;
    bool m_bIsLowHP;

    uint32 m_uiStep;
    uint32 m_uiPhaseTimer;

    GuidList m_luiDwarfGUIDs;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_bIsLowHP = false;
            m_bIsBattle = false;
            m_bHasContinued = false;

            m_uiStep = 0;
            m_uiPhaseTimer = 0;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override                          // TODO - possible better as SummonedJustDied
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_TRIBUNAL, FAIL);
            // Continue at right state after respawn
            if (m_bHasContinued)
                m_pInstance->SetData(TYPE_TRIBUNAL, IN_PROGRESS);
        }

        for (GuidList::const_iterator itr = m_luiDwarfGUIDs.begin(); itr != m_luiDwarfGUIDs.end(); ++itr)
        {
            if (Creature* pDwarf = m_creature->GetMap()->GetCreature(*itr))
                pDwarf->ForcedDespawn();
        }
        m_luiDwarfGUIDs.clear();
    }

    void AttackStart(Unit* pWho) override
    {
        if (!pWho)
            return;

        if (!m_bIsBattle)
            return;

        npc_escortAI::AttackStart(pWho);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        // If Brann takes damage, mark the achiev as failed
        if (uiDamage && m_pInstance)
            m_pInstance->SetBrannSpankin(false);
    }

    void ContinueEvent()
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_TRIBUNAL) != IN_PROGRESS)
            return;

        // Set the achiev in progress
        m_pInstance->SetBrannSpankin(true);

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        SetRun(true);
        SetEscortPaused(false);
        m_bHasContinued = true;
    }

    void JustStartedEscort() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TRIBUNAL, IN_PROGRESS);

        DoScriptText(SAY_ESCORT_START, m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 13:                                        // Before Tribunal Event, Continue with Gossip Interaction
                DoScriptText(SAY_EVENT_INTRO_1, m_creature);
                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 17:                                        // Reach Tribunal
                SetEscortPaused(true);
                m_uiPhaseTimer = 500;
                break;
            case 18:                                        // Reach Floor Event
                SetEscortPaused(true);
                if (m_pInstance)
                {
                    if (GameObject* pKonsole = m_pInstance->GetSingleGameObjectFromStorage(GO_TRIBUNAL_CONSOLE))
                        m_creature->SetFacingToObject(pKonsole);
                    m_pInstance->DoUseDoorOrButton(GO_TRIBUNAL_FLOOR);
                }
                m_uiPhaseTimer = 1000;
                break;
        }
    }

    void SpawnDwarf(uint32 uEntry)
    {
        if (!m_pInstance)
            return;

        // each case has an individual spawn stalker
        switch (uEntry)
        {
            case NPC_DARK_RUNE_PROTECTOR:
            {
                Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetProtectorStalkerGuid());
                if (!pStalker)
                    return;

                uint32 uiSpawnNumber = (m_bIsRegularMode ? 2 : 3);
                for (uint8 i = 0; i < uiSpawnNumber; ++i)
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_PROTECTOR, true, NULL, NULL, m_creature->GetObjectGuid());
                pStalker->CastSpell(pStalker, SPELL_SUMMON_STORMCALLER, true, NULL, NULL, m_creature->GetObjectGuid());
                break;
            }
            case NPC_DARK_RUNE_STORMCALLER:
            {
                Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GeStormcallerStalkerGuid());
                if (!pStalker)
                    return;

                for (uint8 i = 0; i < 2; ++i)
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_STORMCALLER, true, NULL, NULL, m_creature->GetObjectGuid());
                break;
            }
            case NPC_IRON_GOLEM_CUSTODIAN:
            {
                Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetCustodianStalkerGuid());
                if (!pStalker)
                    return;

                pStalker->CastSpell(pStalker, SPELL_SUMMON_CUSTODIAN, true, NULL, NULL, m_creature->GetObjectGuid());
                break;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_luiDwarfGUIDs.push_back(pSummoned->GetObjectGuid());

        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_uiPhaseTimer && m_uiPhaseTimer <= uiDiff)
        {
            switch (m_uiStep)
            {
                    // Begin Event
                case 0:
                    // TODO, this is wrong, must be "using or similar"
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    m_uiPhaseTimer = 1500;
                    break;
                case 1:
                    DoScriptText(SAY_EVENT_INTRO_2, m_creature);
                    m_uiPhaseTimer = 2500;
                    break;
                case 2:
                    if (m_pInstance)
                        m_pInstance->DoUseDoorOrButton(GO_TRIBUNAL_CONSOLE);
                    m_uiPhaseTimer = 6500;
                    break;
                case 3:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_INTRO_3_ABED);
                    m_uiPhaseTimer = 8500;
                    break;

                    // Activate Kaddrak
                case 4:
                    DoScriptText(SAY_EVENT_A_1, m_creature);
                    m_uiPhaseTimer = 6500;
                    break;
                case 5:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_KADDRAK, SAY_EVENT_A_2_KADD);
                    m_uiPhaseTimer = 12500;
                    break;
                case 6:
                    DoScriptText(SAY_EVENT_A_3, m_creature);
                    m_uiPhaseTimer = 6000;
                    break;
                case 7:
                    if (m_pInstance)
                        m_pInstance->ActivateFace(FACE_KADDRAK, false);
                    m_uiPhaseTimer = 5000;
                    break;
                case 8:
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 20000;
                    break;

                    // Activate Marnak
                case 9:
                    DoScriptText(SAY_EVENT_B_1, m_creature);
                    m_uiPhaseTimer = 6000;
                    break;
                case 10:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_MARNAK, SAY_EVENT_B_2_MARN);
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 20000;
                    break;
                case 11:
                    DoScriptText(SAY_EVENT_B_3, m_creature);
                    m_uiPhaseTimer = 5000;
                    break;
                case 12:
                    if (m_pInstance)
                        m_pInstance->ActivateFace(FACE_MARNAK, false);
                    m_uiPhaseTimer = 10000;
                    break;
                case 13:
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 10000;
                    break;
                case 14:
                    SpawnDwarf(NPC_DARK_RUNE_STORMCALLER);
                    m_uiPhaseTimer = (20000);
                    break;
                case 15:
                    DoScriptText(SAY_EVENT_C_1, m_creature);
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 10000;
                    break;
                case 16:
                    SpawnDwarf(NPC_DARK_RUNE_STORMCALLER);
                    m_uiPhaseTimer = 20000;
                    break;

                    // Activate Abedneum
                case 17:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_C_2_ABED);
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 20000;
                    break;
                case 18:
                    DoScriptText(SAY_EVENT_C_3, m_creature);
                    m_uiPhaseTimer = 5000;
                    break;
                case 19:
                    if (m_pInstance)
                        m_pInstance->ActivateFace(FACE_ABEDNEUM, false);
                    m_uiPhaseTimer = 5000;
                    break;
                case 20:
                    SpawnDwarf(NPC_DARK_RUNE_STORMCALLER);
                    m_uiPhaseTimer = 10000;
                    break;
                case 21:
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 15000;
                    break;

                case 22:
                    DoScriptText(SAY_EVENT_D_1, m_creature);
                    SpawnDwarf(NPC_IRON_GOLEM_CUSTODIAN);
                    m_uiPhaseTimer = 20000;
                    break;
                case 23:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_D_2_ABED);
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 5000;
                    break;
                case 24:
                    SpawnDwarf(NPC_DARK_RUNE_STORMCALLER);
                    m_uiPhaseTimer = 15000;
                    break;
                case 25:
                    DoScriptText(SAY_EVENT_D_3, m_creature);
                    SpawnDwarf(NPC_IRON_GOLEM_CUSTODIAN);
                    m_uiPhaseTimer = 5000;
                    break;
                case 26:
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 5000;
                    break;
                case 27:
                    SpawnDwarf(NPC_DARK_RUNE_STORMCALLER);
                    m_uiPhaseTimer = 10000;
                    break;
                case 28:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_D_4_ABED);
                    SpawnDwarf(NPC_DARK_RUNE_PROTECTOR);
                    m_uiPhaseTimer = 10000;
                    break;

                    // End Event
                case 29:
                    DoScriptText(SAY_EVENT_END_01, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);// TODO TODO
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_TRIBUNAL, SPECIAL); // Kill remaining npcs

                    // ToDo: the loot and the achiev should be triggered at this point
                    // Brann should get the gossip option "There will be plenty of time for this later Brann, we need to get moving!"
                    // This will allow Brann to continue the escort to the last encounter
                    // When reaching the last door he has the gossip "We're with you Brann! Open it!"

                    SetEscortPaused(false);
                    m_uiPhaseTimer = 3000;
                    // break;
                    // case 30:
                    if (m_pInstance)
                        m_pInstance->ActivateFace(FACE_ABEDNEUM, true);
                    m_uiPhaseTimer = 0;
                    break;
                case 30:
                    DoScriptText(SAY_EVENT_END_02, m_creature);
                    m_uiPhaseTimer = 5500;
                    break;
                case 31:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_END_03_ABED);
                    m_uiPhaseTimer = 8500;
                    break;
                case 32:
                    DoScriptText(SAY_EVENT_END_04, m_creature);
                    m_uiPhaseTimer = 11500;
                    break;
                case 33:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_END_05_ABED);
                    m_uiPhaseTimer = 11500;
                    break;
                case 34:
                    DoScriptText(SAY_EVENT_END_06, m_creature);
                    m_uiPhaseTimer = 4500;
                    break;
                case 35:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_END_07_ABED);
                    m_uiPhaseTimer = 22500;
                    break;
                case 36:
                    DoScriptText(SAY_EVENT_END_08, m_creature);
                    m_uiPhaseTimer = 7500;
                    break;
                case 37:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_KADDRAK, SAY_EVENT_END_09_KADD);
                    m_uiPhaseTimer = 18500;
                    break;
                case 38:
                    DoScriptText(SAY_EVENT_END_10, m_creature);
                    m_uiPhaseTimer = 5500;
                    break;
                case 39:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_KADDRAK, SAY_EVENT_END_11_KADD);
                    m_uiPhaseTimer = 20500;
                    break;
                case 40:
                    DoScriptText(SAY_EVENT_END_12, m_creature);
                    m_uiPhaseTimer = 2500;
                    break;
                case 41:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_KADDRAK, SAY_EVENT_END_13_KADD);
                    m_uiPhaseTimer = 19500;
                    break;
                case 42:
                    DoScriptText(SAY_EVENT_END_14, m_creature);
                    m_uiPhaseTimer = 10500;
                    break;
                case 43:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_MARNAK, SAY_EVENT_END_15_MARN);
                    m_uiPhaseTimer = 6500;
                    break;
                case 44:
                    DoScriptText(SAY_EVENT_END_16, m_creature);
                    m_uiPhaseTimer = 6500;
                    break;
                case 45:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_MARNAK, SAY_EVENT_END_17_MARN);
                    m_uiPhaseTimer = 25500;
                    break;
                case 46:
                    DoScriptText(SAY_EVENT_END_18, m_creature);
                    m_uiPhaseTimer = 23500;
                    break;
                case 47:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_MARNAK, SAY_EVENT_END_19_MARN);
                    m_uiPhaseTimer = 3500;
                    break;
                case 48:
                    DoScriptText(SAY_EVENT_END_20, m_creature);
                    m_uiPhaseTimer = 8500;
                    break;
                case 49:
                    if (m_pInstance)
                        m_pInstance->DoFaceSpeak(FACE_ABEDNEUM, SAY_EVENT_END_21_ABED);
                    m_uiPhaseTimer = 5500;
                    break;
                case 50:
                {
                    if (m_pInstance)
                    {
                        m_pInstance->DoUseDoorOrButton(GO_TRIBUNAL_FLOOR);
                        m_pInstance->SetData(TYPE_TRIBUNAL, DONE);
                    }

                    Player* pPlayer = GetPlayerForEscort();
                    if (pPlayer)
                        pPlayer->GroupEventHappens(QUEST_HALLS_OF_STONE, m_creature);

                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    m_uiPhaseTimer = 180000;
                    break;
                }
                case 51:
                    SetEscortPaused(false);
                    break;
            }
            ++m_uiStep;
        }
        else if (m_uiPhaseTimer)
            m_uiPhaseTimer -= uiDiff;

        if (!m_bIsLowHP && m_creature->GetHealthPercent() < 30)
        {
            DoScriptText(SAY_LOW_HEALTH, m_creature);
            m_bIsLowHP = true;
        }
        else if (m_bIsLowHP && m_creature->GetHealthPercent() > 30)
            m_bIsLowHP = false;

        // No Combat abilities needed here
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }

    // Respawn Handling: Relocate and Set Escort to WP 13
    void JustRespawned() override
    {
        if (!m_pInstance)
            return;

        Reset();

        if (m_pInstance->GetData(TYPE_TRIBUNAL) == IN_PROGRESS)
        {
            SetEscortPaused(true);

            m_uiStep = 0;
            m_uiPhaseTimer = 0;

            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

            // Relocate to position of WP 13
            m_creature->GetMap()->CreatureRelocation(m_creature, 941.101563f, 377.373413f, 207.421f, 3.85f);

            SetCurrentWaypoint(13);
        }
    }
};

bool GossipHello_npc_brann_hos(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_halls_of_stone* pInstance = (instance_halls_of_stone*)(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_TRIBUNAL) == NOT_STARTED || pInstance->GetData(TYPE_TRIBUNAL) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START, pCreature->GetObjectGuid());
        }
        else if (pInstance->GetData(TYPE_TRIBUNAL) == IN_PROGRESS)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_PROGRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetObjectGuid());
        }
    }

    return true;
}

bool GossipSelect_npc_brann_hos(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (npc_brann_hosAI* pBrannAi = dynamic_cast<npc_brann_hosAI*>(pCreature->AI()))
                pBrannAi->Start(false, pPlayer);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            if (npc_brann_hosAI* pBrannAi = dynamic_cast<npc_brann_hosAI*>(pCreature->AI()))
                pBrannAi->ContinueEvent();
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

CreatureAI* GetAI_npc_brann_hos(Creature* pCreature)
{
    return new npc_brann_hosAI(pCreature);
}

enum
{
    SPELL_SUMMON_DARK_MATTER_TARGET     = 51003,
    SPELL_DARK_MATTER                   = 51012,
    SPELL_DARK_MATTER_H                 = 59868,
    NPC_DARK_MATTER_TARGET              = 28237,

    SPELL_SEARING_GAZE                  = 51136,
    SPELL_SEARING_GAZE_H                = 59867,
    // NPC_SEARING_GAZE_TARGET          = 28265,
};

/*######
## npc_dark_matter
######*/

struct npc_dark_matterAI : public ScriptedAI
{
    npc_dark_matterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_uiSummonTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_DARK_MATTER_START)
            m_uiSummonTimer = 5000;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DARK_MATTER_TARGET)
            m_creature->GetMotionMaster()->MovePoint(1, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ());
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Cast the Dark Matter spell and despawn for reset
        if (DoCastSpellIfCan(m_creature,  m_bIsRegularMode ? SPELL_DARK_MATTER : SPELL_DARK_MATTER_H) == CAST_OK)
        {
            m_creature->SetRespawnDelay(3);
            m_creature->ForcedDespawn(1000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_MATTER_TARGET) == CAST_OK)
                    m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_dark_matter(Creature* pCreature)
{
    return new npc_dark_matterAI(pCreature);
}

/*######
## npc_searing_gaze
######*/

// TODO Move this 'script' to eventAI when combat can be proper prevented from core-side
struct npc_searing_gazeAI : public Scripted_NoMovementAI
{
    npc_searing_gazeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SEARING_GAZE : SPELL_SEARING_GAZE_H);
        // despawn manually because of combat bug
        m_creature->ForcedDespawn(30000);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_searing_gaze(Creature* pCreature)
{
    return new npc_searing_gazeAI(pCreature);
}

void AddSC_halls_of_stone()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_brann_hos";
    pNewScript->GetAI = &GetAI_npc_brann_hos;
    pNewScript->pGossipHello = &GossipHello_npc_brann_hos;
    pNewScript->pGossipSelect = &GossipSelect_npc_brann_hos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_matter";
    pNewScript->GetAI = &GetAI_npc_dark_matter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_searing_gaze";
    pNewScript->GetAI = &GetAI_npc_searing_gaze;
    pNewScript->RegisterSelf();
}
