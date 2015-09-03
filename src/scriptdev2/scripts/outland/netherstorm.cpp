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
SDName: Netherstorm
SD%Complete: 80
SDComment: Quest support: 10191, 10198, 10299, 10310, 10321, 10322, 10323, 10329, 10330, 10337, 10338, 10365(Shutting Down Manaforge), 10406, 10425, 10438, 10924.
SDCategory: Netherstorm
EndScriptData */

/* ContentData
npc_manaforge_control_console
go_manaforge_control_console
npc_commander_dawnforge
npc_bessy
npc_maxx_a_million
npc_zeppit
npc_protectorate_demolitionist
npc_captured_vanguard
npc_drijya
npc_dimensius
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "pet_ai.h"

/*######
## npc_manaforge_control_console
######*/
enum
{
    EMOTE_START                  = -1000211,
    EMOTE_60                     = -1000212,
    EMOTE_30                     = -1000213,
    EMOTE_10                     = -1000214,
    EMOTE_COMPLETE               = -1000215,
    EMOTE_ABORT                  = -1000216,

    NPC_BNAAR_C_CONSOLE          = 20209,
    NPC_CORUU_C_CONSOLE          = 20417,
    NPC_DURO_C_CONSOLE           = 20418,
    NPC_ARA_C_CONSOLE            = 20440,

    NPC_SUNFURY_TECH             = 20218,
    NPC_SUNFURY_PROT             = 20436,

    NPC_ARA_TECH                 = 20438,
    NPC_ARA_ENGI                 = 20439,
    NPC_ARA_GORKLONN             = 20460,

    QUEST_SHUTDOWN_BNAAR_ALDOR   = 10299,
    QUEST_SHUTDOWN_BNAAR_SCRYERS = 10329,
    QUEST_SHUTDOWN_CORUU_ALDOR   = 10321,
    QUEST_SHUTDOWN_CORUU_SCRYERS = 10330,
    QUEST_SHUTDOWN_DURO_ALDOR    = 10322,
    QUEST_SHUTDOWN_DURO_SCRYERS  = 10338,
    QUEST_SHUTDOWN_ARA_ALDOR     = 10323,
    QUEST_SHUTDOWN_ARA_SCRYERS   = 10365,

    ITEM_BNAAR_ACESS_CRYSTAL     = 29366,
    ITEM_CORUU_ACESS_CRYSTAL     = 29396,
    ITEM_DURO_ACESS_CRYSTAL      = 29397,
    ITEM_ARA_ACESS_CRYSTAL       = 29411,

    SPELL_DISABLE_VISUAL         = 35031,
    SPELL_INTERRUPT_1            = 35016,                   // ACID mobs should cast this
    SPELL_INTERRUPT_2            = 35176,                   // ACID mobs should cast this (Manaforge Ara-version)
};

struct npc_manaforge_control_consoleAI : public ScriptedAI
{
    npc_manaforge_control_consoleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_playerGuid;
    ObjectGuid m_consoleGuid;
    uint32 m_uiEventTimer;
    uint32 m_uiWaveTimer;
    uint32 m_uiPhase;
    bool   m_bWave;

    void Reset() override
    {
        m_playerGuid.Clear();
        m_consoleGuid.Clear();
        m_uiEventTimer = 3000;
        m_uiWaveTimer = 0;
        m_uiPhase = 1;
        m_bWave = false;
    }

    /*void SpellHit(Unit *caster, const SpellEntry *spell) override
    {
        // we have no way of telling the creature was hit by spell -> got aura applied after 10-12 seconds
        // then no way for the mobs to actually stop the shutdown as intended.
        if (spell->Id == SPELL_INTERRUPT_1)
            ...
    }*/

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(EMOTE_ABORT, m_creature);

        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (pPlayer)
        {
            switch (m_creature->GetEntry())
            {
                case NPC_BNAAR_C_CONSOLE:
                    pPlayer->FailQuest(QUEST_SHUTDOWN_BNAAR_ALDOR);
                    pPlayer->FailQuest(QUEST_SHUTDOWN_BNAAR_SCRYERS);
                    break;
                case NPC_CORUU_C_CONSOLE:
                    pPlayer->FailQuest(QUEST_SHUTDOWN_CORUU_ALDOR);
                    pPlayer->FailQuest(QUEST_SHUTDOWN_CORUU_SCRYERS);
                    break;
                case NPC_DURO_C_CONSOLE:
                    pPlayer->FailQuest(QUEST_SHUTDOWN_DURO_ALDOR);
                    pPlayer->FailQuest(QUEST_SHUTDOWN_DURO_SCRYERS);
                    break;
                case NPC_ARA_C_CONSOLE:
                    pPlayer->FailQuest(QUEST_SHUTDOWN_ARA_ALDOR);
                    pPlayer->FailQuest(QUEST_SHUTDOWN_ARA_SCRYERS);
                    break;
            }
        }

        if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_consoleGuid))
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    }

    void DoWaveSpawnForCreature(Creature* pCreature)
    {
        Creature* pAdd = NULL;

        switch (pCreature->GetEntry())
        {
            case NPC_BNAAR_C_CONSOLE:
                if (urand(0, 1))
                {
                    if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2933.68f, 4162.55f, 164.00f, 1.60f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 2927.36f, 4212.97f, 164.00f);
                }
                else
                {
                    if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2927.36f, 4212.97f, 164.00f, 4.94f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 2933.68f, 4162.55f, 164.00f);
                }
                m_uiWaveTimer = 30000;
                break;
            case NPC_CORUU_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2445.21f, 2765.26f, 134.49f, 3.93f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2424.21f, 2740.15f, 133.81f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2429.86f, 2731.85f, 134.53f, 1.31f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2435.37f, 2766.04f, 133.81f);
                m_uiWaveTimer = 20000;
                break;
            case NPC_DURO_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2986.80f, 2205.36f, 165.37f, 3.74f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2985.15f, 2197.32f, 164.79f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2952.91f, 2191.20f, 165.32f, 0.22f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2060.01f, 2185.27f, 164.67f);
                m_uiWaveTimer = 15000;
                break;
            case NPC_ARA_C_CONSOLE:
                if (urand(0, 1))
                {
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4035.11f, 4038.97f, 194.27f, 2.57f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4033.66f, 4036.79f, 194.28f, 2.57f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4037.13f, 4037.30f, 194.23f, 2.57f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                }
                else
                {
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3099.59f, 4049.30f, 194.22f, 0.05f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3999.72f, 4046.75f, 194.22f, 0.05f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                    if (pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3996.81f, 4048.26f, 194.22f, 0.05f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                }
                m_uiWaveTimer = 15000;
                break;
        }
    }

    void DoFinalSpawnForCreature(Creature* pCreature)
    {
        Creature* pAdd = NULL;

        switch (pCreature->GetEntry())
        {
            case NPC_BNAAR_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2946.52f, 4201.42f, 163.47f, 3.54f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2927.49f, 4192.81f, 163.00f);
                break;
            case NPC_CORUU_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2453.88f, 2737.85f, 133.27f, 2.59f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2441.62f, 2735.32f, 134.49f, 1.97f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2450.73f, 2754.50f, 134.49f, 3.29f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
                break;
            case NPC_DURO_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2956.18f, 2202.85f, 165.32f, 5.45f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2975.30f, 2211.50f, 165.32f, 4.55f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
                if (pAdd = m_creature->SummonCreature(NPC_SUNFURY_PROT, 2965.02f, 2217.45f, 164.16f, 4.96f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
                break;
            case NPC_ARA_C_CONSOLE:
                if (pAdd = m_creature->SummonCreature(NPC_ARA_ENGI, 3994.51f, 4020.46f, 192.18f, 0.91f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 4008.35f, 4035.04f, 192.70f);
                if (pAdd = m_creature->SummonCreature(NPC_ARA_GORKLONN, 4021.56f, 4059.35f, 193.59f, 4.44f, TEMPSUMMON_TIMED_OOC_DESPAWN, 120000))
                    pAdd->GetMotionMaster()->MovePoint(0, 4016.62f, 4039.89f, 193.46f);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEventTimer < uiDiff)
        {
            Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

            if (!pPlayer)
            {
                // Reset Event
                if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_consoleGuid))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

                m_creature->ForcedDespawn();
                return;
            }

            switch (m_uiPhase)
            {
                case 1:
                    DoScriptText(EMOTE_START, m_creature, pPlayer);
                    m_uiEventTimer = 60000;
                    m_bWave = true;
                    ++m_uiPhase;
                    break;
                case 2:
                    DoScriptText(EMOTE_60, m_creature, pPlayer);
                    m_uiEventTimer = 30000;
                    ++m_uiPhase;
                    break;
                case 3:
                    DoScriptText(EMOTE_30, m_creature, pPlayer);
                    m_uiEventTimer = 20000;
                    DoFinalSpawnForCreature(m_creature);
                    ++m_uiPhase;
                    break;
                case 4:
                    DoScriptText(EMOTE_10, m_creature, pPlayer);
                    m_uiEventTimer = 10000;
                    m_bWave = false;
                    ++m_uiPhase;
                    break;
                case 5:
                    DoScriptText(EMOTE_COMPLETE, m_creature, pPlayer);
                    pPlayer->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
                    DoCastSpellIfCan(m_creature, SPELL_DISABLE_VISUAL);

                    if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_consoleGuid))
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

                    ++m_uiPhase;
                    break;
            }
        }
        else
            m_uiEventTimer -= uiDiff;

        if (m_bWave)
        {
            if (m_uiWaveTimer < uiDiff)
            {
                DoWaveSpawnForCreature(m_creature);
            }
            else
                m_uiWaveTimer -= uiDiff;
        }
    }
};
CreatureAI* GetAI_npc_manaforge_control_console(Creature* pCreature)
{
    return new npc_manaforge_control_consoleAI(pCreature);
}

/*######
## go_manaforge_control_console
######*/

// TODO: clean up this workaround when mangos adds support to do it properly (with gossip selections instead of instant summon)
bool GOUse_go_manaforge_control_console(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        pPlayer->PrepareQuestMenu(pGo->GetObjectGuid());
        pPlayer->SendPreparedQuest(pGo->GetObjectGuid());
    }

    Creature* pManaforge = NULL;

    switch (pGo->GetAreaId())
    {
        case 3726:                                          // b'naar
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_BNAAR_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_BNAAR_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_BNAAR_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_BNAAR_C_CONSOLE, 2918.95f, 4189.98f, 161.88f, 0.34f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3730:                                          // coruu
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_CORUU_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_CORUU_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_CORUU_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_CORUU_C_CONSOLE, 2426.77f, 2750.38f, 133.24f, 2.14f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3734:                                          // duro
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_DURO_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_DURO_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_DURO_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_DURO_C_CONSOLE, 2976.48f, 2183.29f, 163.20f, 1.85f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3722:                                          // ara
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_ARA_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_ARA_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_ARA_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_ARA_C_CONSOLE, 4013.71f, 4028.76f, 192.10f, 1.25f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
    }

    if (pManaforge)
    {
        if (npc_manaforge_control_consoleAI* pManaforgeAI = dynamic_cast<npc_manaforge_control_consoleAI*>(pManaforge->AI()))
        {
            pManaforgeAI->m_playerGuid = pPlayer->GetObjectGuid();
            pManaforgeAI->m_consoleGuid = pGo->GetObjectGuid();
        }

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    }
    return true;
}

/*######
## npc_commander_dawnforge
######*/

// The Speech of Dawnforge, Ardonis & Pathaleon
enum
{
    SAY_COMMANDER_DAWNFORGE_1              = -1000128,
    SAY_ARCANIST_ARDONIS_1                 = -1000129,
    SAY_COMMANDER_DAWNFORGE_2              = -1000130,
    SAY_PATHALEON_THE_CALCULATOR_IMAGE_1   = -1000131,
    SAY_COMMANDER_DAWNFORGE_3              = -1000132,
    SAY_PATHALEON_THE_CALCULATOR_IMAGE_2   = -1000133,
    SAY_PATHALEON_THE_CALCULATOR_IMAGE_2_1 = -1000134,
    SAY_PATHALEON_THE_CALCULATOR_IMAGE_2_2 = -1000135,
    SAY_COMMANDER_DAWNFORGE_4              = -1000136,
    SAY_ARCANIST_ARDONIS_2                 = -1000136,
    SAY_COMMANDER_DAWNFORGE_5              = -1000137,

    QUEST_INFO_GATHERING                   = 10198,
    SPELL_SUNFURY_DISGUISE                 = 34603,

    NPC_ARCANIST_ARDONIS                   = 19830,
    NPC_COMMANDER_DAWNFORGE                = 19831,
    NPC_PATHALEON_THE_CALCULATOR_IMAGE     = 21504
};

struct npc_commander_dawnforgeAI : public ScriptedAI
{
    npc_commander_dawnforgeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;
    ObjectGuid m_ardonisGuid;
    ObjectGuid m_pathaleonGuid;

    uint32 m_uiPhase;
    uint32 m_uiPhaseSubphase;
    uint32 m_uiPhaseTimer;
    bool   m_bIsEvent;

    void Reset() override
    {
        m_playerGuid.Clear();
        m_ardonisGuid.Clear();
        m_pathaleonGuid.Clear();

        m_uiPhase = 1;
        m_uiPhaseSubphase = 0;
        m_uiPhaseTimer = 4000;
        m_bIsEvent = false;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_PATHALEON_THE_CALCULATOR_IMAGE)
            m_pathaleonGuid = pSummoned->GetObjectGuid();
    }

    void TurnToPathaleonsImage()
    {
        Creature* pArdonis = m_creature->GetMap()->GetCreature(m_ardonisGuid);
        Creature* pPathaleon = m_creature->GetMap()->GetCreature(m_pathaleonGuid);
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (!pArdonis || !pPathaleon || !pPlayer)
            return;

        m_creature->SetFacingToObject(pPathaleon);
        pArdonis->SetFacingToObject(pPathaleon);

        // the boss is there kneel before him
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        pArdonis->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

    void TurnToEachOther()
    {
        if (Creature* pArdonis = m_creature->GetMap()->GetCreature(m_ardonisGuid))
        {
            Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

            if (!pPlayer)
                return;

            m_creature->SetFacingToObject(pArdonis);
            pArdonis->SetFacingToObject(m_creature);

            // get up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            pArdonis->SetStandState(UNIT_STAND_STATE_STAND);
        }
    }

    bool CanStartEvent(Player* pPlayer)
    {
        if (!m_bIsEvent)
        {
            Creature* pArdonis = GetClosestCreatureWithEntry(m_creature, NPC_ARCANIST_ARDONIS, 10.0f);

            if (!pArdonis)
                return false;

            m_ardonisGuid = pArdonis->GetObjectGuid();
            m_playerGuid = pPlayer->GetObjectGuid();

            m_bIsEvent = true;

            TurnToEachOther();
            return true;
        }

        debug_log("SD2: npc_commander_dawnforge event already in progress, need to wait.");
        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Is event even running?
        if (!m_bIsEvent)
            return;

        // Phase timing
        if (m_uiPhaseTimer >= uiDiff)
        {
            m_uiPhaseTimer -= uiDiff;
            return;
        }

        Creature* pArdonis = m_creature->GetMap()->GetCreature(m_ardonisGuid);
        Creature* pPathaleon = m_creature->GetMap()->GetCreature(m_pathaleonGuid);
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (!pArdonis || !pPlayer)
        {
            Reset();
            return;
        }

        if (m_uiPhase > 4 && !pPathaleon)
        {
            Reset();
            return;
        }

        switch (m_uiPhase)
        {
            case 1:
                DoScriptText(SAY_COMMANDER_DAWNFORGE_1, m_creature);
                ++m_uiPhase;
                m_uiPhaseTimer = 16000;
                break;
            case 2:
                DoScriptText(SAY_ARCANIST_ARDONIS_1, pArdonis);
                ++m_uiPhase;
                m_uiPhaseTimer = 16000;
                break;
            case 3:
                DoScriptText(SAY_COMMANDER_DAWNFORGE_2, m_creature);
                ++m_uiPhase;
                m_uiPhaseTimer = 16000;
                break;
            case 4:
                // spawn pathaleon's image
                m_creature->SummonCreature(NPC_PATHALEON_THE_CALCULATOR_IMAGE, 2325.851563f, 2799.534668f, 133.084229f, 6.038996f, TEMPSUMMON_TIMED_DESPAWN, 90000);
                ++m_uiPhase;
                m_uiPhaseTimer = 500;
                break;
            case 5:
                DoScriptText(SAY_PATHALEON_THE_CALCULATOR_IMAGE_1, pPathaleon);
                ++m_uiPhase;
                m_uiPhaseTimer = 6000;
                break;
            case 6:
                switch (m_uiPhaseSubphase)
                {
                    case 0:
                        TurnToPathaleonsImage();
                        ++m_uiPhaseSubphase;
                        m_uiPhaseTimer = 8000;
                        break;
                    case 1:
                        DoScriptText(SAY_COMMANDER_DAWNFORGE_3, m_creature);
                        m_uiPhaseSubphase = 0;
                        ++m_uiPhase;
                        m_uiPhaseTimer = 8000;
                        break;
                }
                break;
            case 7:
                switch (m_uiPhaseSubphase)
                {
                    case 0:
                        DoScriptText(SAY_PATHALEON_THE_CALCULATOR_IMAGE_2, pPathaleon);
                        ++m_uiPhaseSubphase;
                        m_uiPhaseTimer = 12000;
                        break;
                    case 1:
                        DoScriptText(SAY_PATHALEON_THE_CALCULATOR_IMAGE_2_1, pPathaleon);
                        ++m_uiPhaseSubphase;
                        m_uiPhaseTimer = 16000;
                        break;
                    case 2:
                        DoScriptText(SAY_PATHALEON_THE_CALCULATOR_IMAGE_2_2, pPathaleon);
                        m_uiPhaseSubphase = 0;
                        ++m_uiPhase;
                        m_uiPhaseTimer = 10000;
                        break;
                }
                break;
            case 8:
                DoScriptText(SAY_COMMANDER_DAWNFORGE_4, m_creature);
                DoScriptText(SAY_ARCANIST_ARDONIS_2, pArdonis);
                ++m_uiPhase;
                m_uiPhaseTimer = 4000;
                break;
            case 9:
                TurnToEachOther();
                // hide pathaleon, unit will despawn shortly
                pPathaleon->SetVisibility(VISIBILITY_OFF);
                m_uiPhaseSubphase = 0;
                ++m_uiPhase;
                m_uiPhaseTimer = 3000;
                break;
            case 10:
                DoScriptText(SAY_COMMANDER_DAWNFORGE_5, m_creature);
                pPlayer->AreaExploredOrEventHappens(QUEST_INFO_GATHERING);
                Reset();
                break;
        }
    }
};

CreatureAI* GetAI_npc_commander_dawnforge(Creature* pCreature)
{
    return new npc_commander_dawnforgeAI(pCreature);
}

bool AreaTrigger_at_commander_dawnforge(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    // if player lost aura or not have at all, we should not try start event.
    if (!pPlayer->HasAura(SPELL_SUNFURY_DISGUISE, EFFECT_INDEX_0))
        return false;

    if (pPlayer->isAlive() && pPlayer->GetQuestStatus(QUEST_INFO_GATHERING) == QUEST_STATUS_INCOMPLETE)
    {
        Creature* pDawnforge = GetClosestCreatureWithEntry(pPlayer, NPC_COMMANDER_DAWNFORGE, 30.0f);

        if (!pDawnforge)
            return false;

        if (npc_commander_dawnforgeAI* pDawnforgeAI = dynamic_cast<npc_commander_dawnforgeAI*>(pDawnforge->AI()))
        {
            pDawnforgeAI->CanStartEvent(pPlayer);
            return true;
        }
    }
    return false;
}

/*######
## npc_bessy
######*/

enum
{
    QUEST_COWS_COME_HOME = 10337,

    NPC_THADELL          = 20464,
    NPC_TORMENTED_SOUL   = 20512,
    NPC_SEVERED_SPIRIT   = 19881
};

struct npc_bessyAI : public npc_escortAI
{
    npc_bessyAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 3:
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.67f, 2183.11f, 96.85f, 6.20f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.53f, 2184.43f, 96.36f, 6.27f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.85f, 2186.34f, 97.57f, 6.08f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                break;
            case 7:
                m_creature->SummonCreature(NPC_SEVERED_SPIRIT, 2309.64f, 2186.24f, 92.25f, 6.06f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_SEVERED_SPIRIT, 2309.25f, 2183.46f, 91.75f, 6.22f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                break;
            case 12:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_COWS_COME_HOME, m_creature);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void Reset() override {}
};

bool QuestAccept_npc_bessy(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_COWS_COME_HOME)
    {
        pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_NON_ATTACKABLE);

        if (npc_bessyAI* pBessyAI = dynamic_cast<npc_bessyAI*>(pCreature->AI()))
            pBessyAI->Start(true, pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_bessy(Creature* pCreature)
{
    return new npc_bessyAI(pCreature);
}

/*######
## npc_maxx_a_million
######*/

enum
{
    QUEST_MARK_V_IS_ALIVE       = 10191,
    NPC_BOT_SPECIALIST_ALLEY    = 19578,
    GO_DRAENEI_MACHINE          = 183771,

    SAY_START                   = -1000621,
    SAY_ALLEY_FAREWELL          = -1000622,
    SAY_CONTINUE                = -1000623,
    SAY_ALLEY_FINISH            = -1000624
};

struct npc_maxx_a_million_escortAI : public npc_escortAI
{
    npc_maxx_a_million_escortAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint8 m_uiSubEvent;
    uint32 m_uiSubEventTimer;
    ObjectGuid m_alleyGuid;
    ObjectGuid m_lastDraeneiMachineGuid;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSubEvent = 0;
            m_uiSubEventTimer = 0;
            m_alleyGuid.Clear();
            m_lastDraeneiMachineGuid.Clear();

            // Reset fields, that were changed on escort-start
            m_creature->HandleEmote(EMOTE_STATE_STUN);
        }
    }

    void WaypointReached(uint32 uiPoint) override
    {
        switch (uiPoint)
        {
            case 1:
                // turn 90 degrees , towards doorway.
                m_creature->SetFacingTo(m_creature->GetOrientation() + (M_PI_F / 2));
                DoScriptText(SAY_START, m_creature);
                m_uiSubEventTimer = 3000;
                m_uiSubEvent = 1;
                break;
            case 7:
            case 17:
            case 29:
                if (GameObject* pMachine = GetClosestGameObjectWithEntry(m_creature, GO_DRAENEI_MACHINE, INTERACTION_DISTANCE))
                {
                    m_creature->SetFacingToObject(pMachine);
                    m_lastDraeneiMachineGuid = pMachine->GetObjectGuid();
                    m_uiSubEvent = 2;
                    m_uiSubEventTimer = 1000;
                }
                else
                    m_lastDraeneiMachineGuid.Clear();

                break;
            case 36:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_MARK_V_IS_ALIVE, m_creature);

                if (Creature* pAlley = m_creature->GetMap()->GetCreature(m_alleyGuid))
                    DoScriptText(SAY_ALLEY_FINISH, pAlley);

                break;
        }
    }

    void WaypointStart(uint32 uiPoint) override
    {
        switch (uiPoint)
        {
            case 8:
            case 18:
            case 30:
                DoScriptText(SAY_CONTINUE, m_creature);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() ||  !m_creature->getVictim())
        {
            if (m_uiSubEventTimer)
            {
                if (m_uiSubEventTimer <= uiDiff)
                {
                    switch (m_uiSubEvent)
                    {
                        case 1:                             // Wait time before Say
                            if (Creature* pAlley = GetClosestCreatureWithEntry(m_creature, NPC_BOT_SPECIALIST_ALLEY, INTERACTION_DISTANCE * 2))
                            {
                                m_alleyGuid = pAlley->GetObjectGuid();
                                DoScriptText(SAY_ALLEY_FAREWELL, pAlley);
                            }
                            m_uiSubEventTimer = 0;
                            m_uiSubEvent = 0;
                            break;
                        case 2:                             // Short wait time after reached WP at machine
                            m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
                            m_uiSubEventTimer = 2000;
                            m_uiSubEvent = 3;
                            break;
                        case 3:                             // Despawn machine after 2s
                            if (GameObject* pMachine = m_creature->GetMap()->GetGameObject(m_lastDraeneiMachineGuid))
                                pMachine->Use(m_creature);

                            m_lastDraeneiMachineGuid.Clear();
                            m_uiSubEventTimer = 0;
                            m_uiSubEvent = 0;
                            break;
                        default:
                            m_uiSubEventTimer = 0;
                            break;
                    }
                }
                else
                    m_uiSubEventTimer -= uiDiff;
            }
        }
        else
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_maxx_a_million(Creature* pCreature)
{
    return new npc_maxx_a_million_escortAI(pCreature);
}

bool QuestAccept_npc_maxx_a_million(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MARK_V_IS_ALIVE)
    {
        if (npc_maxx_a_million_escortAI* pEscortAI = dynamic_cast<npc_maxx_a_million_escortAI*>(pCreature->AI()))
        {
            // Set Faction to Escort Faction
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_OOC_NOT_ATTACK | TEMPFACTION_TOGGLE_PASSIVE);
            // Set emote-state to 0 (is EMOTE_STATE_STUN by default)
            pCreature->HandleEmote(EMOTE_ONESHOT_NONE);

            pEscortAI->Start(false, pPlayer, pQuest, true);
        }
    }
    return true;
}

/*######
## npc_zeppit
######*/

enum
{
    EMOTE_GATHER_BLOOD          = -1000625,
    NPC_WARP_CHASER             = 18884,
    SPELL_GATHER_WARP_BLOOD     = 39244,                    // for quest 10924
};

struct npc_zeppitAI : public ScriptedPetAI
{
    npc_zeppitAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    void Reset() override { }

    void OwnerKilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetEntry() == NPC_WARP_CHASER)
        {
            // Distance not known, be assumed to be ~10 yards, possibly a bit less.
            if (m_creature->IsWithinDistInMap(pVictim, 10.0f))
            {
                DoScriptText(EMOTE_GATHER_BLOOD, m_creature);
                m_creature->CastSpell(m_creature, SPELL_GATHER_WARP_BLOOD, false);
            }
        }
    }
};

CreatureAI* GetAI_npc_zeppit(Creature* pCreature)
{
    return new npc_zeppitAI(pCreature);
}

/*######
## npc_protectorate_demolitionist
######*/

enum
{
    SAY_INTRO                       = -1000891,
    SAY_ATTACKED_1                  = -1000892,
    SAY_ATTACKED_2                  = -1000893,
    SAY_STAGING_GROUNDS             = -1000894,
    SAY_TOXIC_HORROR                = -1000895,
    SAY_SALHADAAR                   = -1000896,
    SAY_DISRUPTOR                   = -1000897,
    SAY_NEXUS_PROTECT               = -1000898,
    SAY_FINISH_1                    = -1000899,
    SAY_FINISH_2                    = -1000900,

    SPELL_ETHEREAL_TELEPORT         = 34427,
    SPELL_PROTECTORATE              = 35679,                // dummy aura applied on player

    NPC_NEXUS_STALKER               = 20474,
    NPC_ARCHON                      = 20458,

    FACTION_FRIENDLY                = 35,

    QUEST_ID_DELIVERING_MESSAGE     = 10406,
};

struct npc_protectorate_demolitionistAI : public npc_escortAI
{
    npc_protectorate_demolitionistAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiEventTimer;
    uint8 m_uiEventStage;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiEventTimer = 0;
            m_uiEventStage = 0;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_ATTACKED_1 : SAY_ATTACKED_2, m_creature);
    }

    // No attack done by this npc
    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        // Star the escort
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (pWho->HasAura(SPELL_PROTECTORATE) && ((Player*)pWho)->GetQuestStatus(QUEST_ID_DELIVERING_MESSAGE) == QUEST_STATUS_INCOMPLETE)
            {
                if (m_creature->IsWithinDistInMap(pWho, 10.0f))
                {
                    m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
                    Start(false, (Player*)pWho);
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_NEXUS_STALKER)
            DoScriptText(SAY_NEXUS_PROTECT, pSummoned);
        else if (pSummoned->GetEntry() == NPC_ARCHON)
            pSummoned->CastSpell(pSummoned, SPELL_ETHEREAL_TELEPORT, true);

        pSummoned->AI()->AttackStart(m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_INTRO, m_creature);
                break;
            case 3:
                DoScriptText(SAY_STAGING_GROUNDS, m_creature);
                break;
            case 4:
                DoScriptText(SAY_TOXIC_HORROR, m_creature);
                break;
            case 9:
                DoScriptText(SAY_SALHADAAR, m_creature);
                break;
            case 12:
                DoScriptText(SAY_DISRUPTOR, m_creature);
                SetEscortPaused(true);
                m_uiEventTimer = 5000;
                break;
            case 13:
                DoScriptText(SAY_FINISH_2, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_creature->SetFacingToObject(pPlayer);
                    pPlayer->GroupEventHappens(QUEST_ID_DELIVERING_MESSAGE, m_creature);
                }
                SetEscortPaused(true);
                m_uiEventTimer = 6000;
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventStage)
                {
                    case 0:
                        m_creature->SummonCreature(NPC_ARCHON, 3875.69f, 2308.72f, 115.80f, 1.48f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                        m_uiEventTimer = 4000;
                        break;
                    case 1:
                        m_creature->SummonCreature(NPC_NEXUS_STALKER, 3884.06f, 2325.22f, 111.37f, 3.45f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                        m_creature->SummonCreature(NPC_NEXUS_STALKER, 3861.54f, 2320.44f, 111.48f, 0.32f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                        m_uiEventTimer = 9000;
                        break;
                    case 2:
                        DoScriptText(SAY_FINISH_1, m_creature);
                        SetRun();
                        SetEscortPaused(false);
                        m_uiEventTimer = 0;
                        break;
                    case 3:
                        if (DoCastSpellIfCan(m_creature, SPELL_ETHEREAL_TELEPORT, CAST_TRIGGERED) == CAST_OK)
                            m_creature->ForcedDespawn(1000);
                        m_uiEventTimer = 0;
                        break;
                }
                ++m_uiEventStage;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        // ToDo: research if the npc uses spells or melee for combat
    }
};

CreatureAI* GetAI_npc_protectorate_demolitionist(Creature* pCreature)
{
    return new npc_protectorate_demolitionistAI(pCreature);
}

/*######
## npc_captured_vanguard
######*/

enum
{
    SAY_VANGUARD_INTRO              = -1000901,
    SAY_VANGUARD_START              = -1000902,
    SAY_VANGUARD_FINISH             = -1000903,
    EMOTE_VANGUARD_FINISH           = -1000904,

    SPELL_GLAIVE                    = 36500,
    SPELL_HAMSTRING                 = 31553,

    NPC_COMMANDER_AMEER             = 20448,

    QUEST_ID_ESCAPE_STAGING_GROUNDS = 10425,
};

struct npc_captured_vanguardAI : public npc_escortAI
{
    npc_captured_vanguardAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiGlaiveTimer;
    uint32 m_uiHamstringTimer;

    void Reset() override
    {
        m_uiGlaiveTimer = urand(4000, 8000);
        m_uiHamstringTimer = urand(8000, 13000);
    }

    void JustReachedHome() override
    {
        // Happens only if the player helps the npc in the fight - otherwise he dies
        DoScriptText(SAY_VANGUARD_INTRO, m_creature);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 15:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ID_ESCAPE_STAGING_GROUNDS, m_creature);
                break;
            case 16:
                DoScriptText(SAY_VANGUARD_FINISH, m_creature);
                SetRun();
                break;
            case 17:
                if (Creature* pAmeer = GetClosestCreatureWithEntry(m_creature, NPC_COMMANDER_AMEER, 5.0f))
                    DoScriptText(EMOTE_VANGUARD_FINISH, m_creature, pAmeer);
                break;
            case 18:
                if (DoCastSpellIfCan(m_creature, SPELL_ETHEREAL_TELEPORT, CAST_TRIGGERED) == CAST_OK)
                    m_creature->ForcedDespawn(1000);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiGlaiveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GLAIVE) == CAST_OK)
                m_uiGlaiveTimer = urand(5000, 9000);
        }
        else
            m_uiGlaiveTimer -= uiDiff;

        if (m_uiHamstringTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMSTRING) == CAST_OK)
                m_uiHamstringTimer = urand(10000, 16000);
        }
        else
            m_uiHamstringTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_captured_vanguard(Creature* pCreature)
{
    return new npc_captured_vanguardAI(pCreature);
}

bool QuestAccept_npc_captured_vanguard(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_ESCAPE_STAGING_GROUNDS)
    {
        if (npc_captured_vanguardAI* pEscortAI = dynamic_cast<npc_captured_vanguardAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);

        DoScriptText(SAY_VANGUARD_START, pCreature, pPlayer);
    }

    return true;
}

/*######
## npc_drijya
######*/

enum
{
    SAY_DRIJYA_START        = -1000968,
    SAY_DRIJYA_1            = -1000969,
    SAY_DRIJYA_2            = -1000970,
    SAY_DRIJYA_3            = -1000971,
    SAY_DRIJYA_4            = -1000972,
    SAY_DRIJYA_5            = -1000973,
    SAY_DRIJYA_6            = -1000974,
    SAY_DRIJYA_7            = -1000975,
    SAY_DRIJYA_COMPLETE     = -1000976,

    SPELL_SUMMON_SMOKE      = 42456,                        // summon temp GO 185318
    SPELL_SUMMON_FIRE       = 42467,                        // summon temp GO 185319
    SPELL_EXPLOSION_VISUAL  = 42458,

    NPC_EXPLODE_TRIGGER     = 20296,
    NPC_TERROR_IMP          = 20399,
    NPC_LEGION_TROOPER      = 20402,
    NPC_LEGION_DESTROYER    = 20403,

    // GO_SMOKE             = 185318,
    // GO_FIRE              = 185317,                       // not sure if this one is used
    // GO_BIG_FIRE          = 185319,

    QUEST_ID_WARP_GATE      = 10310,

    MAX_TROOPERS            = 9,
    MAX_IMPS                = 6,
};

struct npc_drijyaAI : public npc_escortAI
{
    npc_drijyaAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint8 m_uiSpawnCount;
    uint32 m_uiSpawnImpTimer;
    uint32 m_uiSpawnTrooperTimer;
    uint32 m_uiSpawnDestroyerTimer;
    uint32 m_uiDestroyingTimer;

    ObjectGuid m_explodeTriggerGuid;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSpawnCount          = 0;
            m_uiSpawnImpTimer       = 0;
            m_uiSpawnTrooperTimer   = 0;
            m_uiSpawnDestroyerTimer = 0;
            m_uiDestroyingTimer     = 0;
        }
    }

    void AttackedBy(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_TERROR_IMP || pWho->GetEntry() == NPC_LEGION_TROOPER || pWho->GetEntry() == NPC_LEGION_DESTROYER)
        {
            if (urand(0, 1))
                DoScriptText(SAY_DRIJYA_3, m_creature);
        }
    }

    void DoSpawnCreature(uint32 uiEntry)
    {
        if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_explodeTriggerGuid))
            m_creature->SummonCreature(uiEntry, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), pTrigger->GetOrientation(), TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_TERROR_IMP:
            case NPC_LEGION_TROOPER:
            case NPC_LEGION_DESTROYER:
                pSummoned->AI()->AttackStart(m_creature);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_DRIJYA_START, m_creature);
                SetRun();
                break;
            case 1:
                DoScriptText(SAY_DRIJYA_1, m_creature);
                break;
            case 5:
                DoScriptText(SAY_DRIJYA_2, m_creature);
                break;
            case 7:
                SetEscortPaused(true);
                m_uiDestroyingTimer = 60000;
                m_uiSpawnImpTimer = 15000;
                m_uiSpawnCount = 0;
                m_creature->HandleEmoteCommand(EMOTE_STATE_WORK);
                if (Creature* pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_EXPLODE_TRIGGER, 30.0f))
                    m_explodeTriggerGuid = pTrigger->GetObjectGuid();
                break;
            case 8:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOKE) == CAST_OK)
                {
                    if (Player* pPlayer = GetPlayerForEscort())
                        m_creature->SetFacingToObject(pPlayer);

                    DoScriptText(SAY_DRIJYA_4, m_creature);
                }
                break;
            case 12:
                SetEscortPaused(true);
                m_uiDestroyingTimer = 60000;
                m_uiSpawnTrooperTimer = 15000;
                m_uiSpawnCount = 0;
                m_creature->HandleEmoteCommand(EMOTE_STATE_WORK);
                break;
            case 13:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOKE) == CAST_OK)
                {
                    if (Player* pPlayer = GetPlayerForEscort())
                        m_creature->SetFacingToObject(pPlayer);

                    DoScriptText(SAY_DRIJYA_5, m_creature);
                }
                break;
            case 17:
                SetEscortPaused(true);
                m_uiDestroyingTimer = 60000;
                m_uiSpawnDestroyerTimer = 15000;
                m_creature->HandleEmoteCommand(EMOTE_STATE_WORK);
                break;
            case 18:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOKE) == CAST_OK)
                {
                    if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_explodeTriggerGuid))
                        m_creature->SetFacingToObject(pTrigger);

                    DoScriptText(SAY_DRIJYA_6, m_creature);
                }
                break;
            case 19:
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_explodeTriggerGuid))
                {
                    pTrigger->CastSpell(pTrigger, SPELL_SUMMON_FIRE, true);
                    pTrigger->CastSpell(pTrigger, SPELL_EXPLOSION_VISUAL, true);
                }
                break;
            case 20:
                DoScriptText(SAY_DRIJYA_7, m_creature);
                break;
            case 23:
                SetRun(false);
                break;
            case 27:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_DRIJYA_COMPLETE, m_creature, pPlayer);
                    pPlayer->GroupEventHappens(QUEST_ID_WARP_GATE, m_creature);
                }
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue), true);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_uiSpawnImpTimer)
        {
            if (m_uiSpawnImpTimer <= uiDiff)
            {
                DoSpawnCreature(NPC_TERROR_IMP);
                ++m_uiSpawnCount;

                if (m_uiSpawnCount == MAX_IMPS)
                    m_uiSpawnImpTimer = 0;
                else
                    m_uiSpawnImpTimer = 3500;
            }
            else
                m_uiSpawnImpTimer -= uiDiff;
        }

        if (m_uiSpawnTrooperTimer)
        {
            if (m_uiSpawnTrooperTimer <= uiDiff)
            {
                DoSpawnCreature(NPC_LEGION_TROOPER);
                ++m_uiSpawnCount;

                if (m_uiSpawnCount == MAX_TROOPERS)
                    m_uiSpawnTrooperTimer = 0;
                else
                    m_uiSpawnTrooperTimer = 3500;
            }
            else
                m_uiSpawnTrooperTimer -= uiDiff;
        }

        if (m_uiSpawnDestroyerTimer)
        {
            if (m_uiSpawnDestroyerTimer <= uiDiff)
            {
                DoSpawnCreature(NPC_LEGION_DESTROYER);
                m_uiSpawnDestroyerTimer = 0;
            }
            else
                m_uiSpawnDestroyerTimer -= uiDiff;
        }

        if (m_uiDestroyingTimer)
        {
            if (m_uiDestroyingTimer <= uiDiff)
            {
                SetEscortPaused(false);
                m_creature->HandleEmoteCommand(EMOTE_STATE_NONE);
                m_uiDestroyingTimer = 0;
            }
            else
                m_uiDestroyingTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_drijya(Creature* pCreature)
{
    return new npc_drijyaAI(pCreature);
}

bool QuestAccept_npc_drijya(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_WARP_GATE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_dimensius
######*/

enum
{
    SAY_AGGRO                   = -1001170,
    SAY_SUMMON                  = -1001171,

    SPELL_DIMENSIUS_FEEDING     = 37450,
    SPELL_SHADOW_SPIRAL         = 37500,
    SPELL_SHADOW_VAULT          = 37412,

    NPC_SPAWN_OF_DIMENSIUS      = 21780,
    NPC_CAPTAIN_SAEED           = 20985,
    MODEL_ID_DIMENSIUS_CLOUD    = 20011,
};

// order based on the increasing range of damage
static const uint32 auiShadowRainSpells[5] = { 37399, 37405, 37397, 37396, 37409 };

struct npc_dimensiusAI : public Scripted_NoMovementAI
{
    npc_dimensiusAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiSpiralTimer;
    uint32 m_uiVaultTimer;
    uint32 m_uiRainTimer;
    uint8 m_uiRainIndex;
    uint8 m_uiSpawnsDead;

    bool m_bSpawnsFeeding;

    void Reset() override
    {
        m_uiSpiralTimer = 1000;
        m_uiVaultTimer  = urand(5000, 10000);
        m_uiRainTimer   = 0;
        m_uiRainIndex   = urand(0, 4);
        m_uiSpawnsDead  = 0;

        m_bSpawnsFeeding = false;

        m_creature->SetDisplayId(MODEL_ID_DIMENSIUS_CLOUD);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPAWN_OF_DIMENSIUS)
            pSummoned->CastSpell(m_creature, SPELL_DIMENSIUS_FEEDING, true);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPAWN_OF_DIMENSIUS)
        {
            // interrupt the shadow rain when all spawns are dead
            ++m_uiSpawnsDead;
            if (m_uiSpawnsDead == 4)
            {
                m_creature->InterruptNonMeleeSpells(false);
                m_uiRainTimer = 0;
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // event is sent by dbscript
        if (eventType == AI_EVENT_CUSTOM_EVENTAI_B && pSender->GetEntry() == NPC_CAPTAIN_SAEED)
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bSpawnsFeeding && m_creature->GetHealthPercent() < 75.0f)
        {
            DoScriptText(SAY_SUMMON, m_creature);

            float fX, fY, fZ;
            for (uint8 i = 0; i < 4; ++i)
            {
                m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 30.0f, i * (M_PI_F / 2));
                m_creature->SummonCreature(NPC_SPAWN_OF_DIMENSIUS, fX, fY, fZ, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
            }

            m_uiRainTimer = 5000;
            m_bSpawnsFeeding = true;
        }

        if (m_uiRainTimer)
        {
            if (m_uiRainTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, auiShadowRainSpells[m_uiRainIndex], CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    m_uiRainIndex = urand(0, 4);
                    m_uiRainTimer = 5000;
                }
            }
            else
                m_uiRainTimer -= uiDiff;

            return;
        }

        if (m_uiSpiralTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_SPIRAL) == CAST_OK)
                    m_uiSpiralTimer = urand(3000, 4000);
            }
        }
        else
            m_uiSpiralTimer -= uiDiff;

        if (m_uiVaultTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_VAULT) == CAST_OK)
                    m_uiVaultTimer = urand(20000, 30000);
            }
        }
        else
            m_uiVaultTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_dimensius(Creature* pCreature)
{
    return new npc_dimensiusAI(pCreature);
}

void AddSC_netherstorm()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_manaforge_control_console";
    pNewScript->pGOUse = &GOUse_go_manaforge_control_console;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_manaforge_control_console";
    pNewScript->GetAI = &GetAI_npc_manaforge_control_console;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_dawnforge";
    pNewScript->GetAI = &GetAI_npc_commander_dawnforge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_commander_dawnforge";
    pNewScript->pAreaTrigger = &AreaTrigger_at_commander_dawnforge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bessy";
    pNewScript->GetAI = &GetAI_npc_bessy;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_bessy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_maxx_a_million";
    pNewScript->GetAI = &GetAI_npc_maxx_a_million;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_maxx_a_million;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_zeppit";
    pNewScript->GetAI = &GetAI_npc_zeppit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_protectorate_demolitionist";
    pNewScript->GetAI = &GetAI_npc_protectorate_demolitionist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_captured_vanguard";
    pNewScript->GetAI = &GetAI_npc_captured_vanguard;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_captured_vanguard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_drijya";
    pNewScript->GetAI = &GetAI_npc_drijya;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_drijya;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dimensius";
    pNewScript->GetAI = &GetAI_npc_dimensius;
    pNewScript->RegisterSelf();
}
