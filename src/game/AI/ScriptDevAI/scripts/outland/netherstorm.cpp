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

#include "AI/ScriptDevAI/include/precompiled.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"

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
        Creature* pAdd = nullptr;

        switch (pCreature->GetEntry())
        {
            case NPC_BNAAR_C_CONSOLE:
                if (urand(0, 1))
                {
                    pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2933.68f, 4162.55f, 164.00f, 1.60f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 2927.36f, 4212.97f, 164.00f);
                }
                else
                {
                    pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2927.36f, 4212.97f, 164.00f, 4.94f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 2933.68f, 4162.55f, 164.00f);
                }
                m_uiWaveTimer = 30000;
                break;
            case NPC_CORUU_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2445.21f, 2765.26f, 134.49f, 3.93f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2424.21f, 2740.15f, 133.81f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2429.86f, 2731.85f, 134.53f, 1.31f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2435.37f, 2766.04f, 133.81f);
                m_uiWaveTimer = 20000;
                break;
            case NPC_DURO_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2986.80f, 2205.36f, 165.37f, 3.74f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2985.15f, 2197.32f, 164.79f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2952.91f, 2191.20f, 165.32f, 0.22f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2060.01f, 2185.27f, 164.67f);
                m_uiWaveTimer = 15000;
                break;
            case NPC_ARA_C_CONSOLE:
                if (urand(0, 1))
                {
                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4035.11f, 4038.97f, 194.27f, 2.57f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);

                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4033.66f, 4036.79f, 194.28f, 2.57f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);

                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 4037.13f, 4037.30f, 194.23f, 2.57f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                }
                else
                {
                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3099.59f, 4049.30f, 194.22f, 0.05f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);

                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3999.72f, 4046.75f, 194.22f, 0.05f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);

                    pAdd = m_creature->SummonCreature(NPC_ARA_TECH, 3996.81f, 4048.26f, 194.22f, 0.05f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                    if (pAdd)
                        pAdd->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                }
                m_uiWaveTimer = 15000;
                break;
        }
    }

    void DoFinalSpawnForCreature(Creature* pCreature)
    {
        Creature* pAdd = nullptr;

        switch (pCreature->GetEntry())
        {
            case NPC_BNAAR_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2946.52f, 4201.42f, 163.47f, 3.54f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2927.49f, 4192.81f, 163.00f);
                break;
            case NPC_CORUU_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2453.88f, 2737.85f, 133.27f, 2.59f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2441.62f, 2735.32f, 134.49f, 1.97f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2450.73f, 2754.50f, 134.49f, 3.29f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
                break;
            case NPC_DURO_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2956.18f, 2202.85f, 165.32f, 5.45f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_TECH, 2975.30f, 2211.50f, 165.32f, 4.55f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);

                pAdd = m_creature->SummonCreature(NPC_SUNFURY_PROT, 2965.02f, 2217.45f, 164.16f, 4.96f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
                break;
            case NPC_ARA_C_CONSOLE:
                pAdd = m_creature->SummonCreature(NPC_ARA_ENGI, 3994.51f, 4020.46f, 192.18f, 0.91f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
                    pAdd->GetMotionMaster()->MovePoint(0, 4008.35f, 4035.04f, 192.70f);

                pAdd = m_creature->SummonCreature(NPC_ARA_GORKLONN, 4021.56f, 4059.35f, 193.59f, 4.44f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
                if (pAdd)
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

UnitAI* GetAI_npc_manaforge_control_console(Creature* pCreature)
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

    Creature* pManaforge = nullptr;

    switch (pGo->GetAreaId())
    {
        case 3726:                                          // b'naar
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_BNAAR_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_BNAAR_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_BNAAR_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_BNAAR_C_CONSOLE, 2918.95f, 4189.98f, 161.88f, 0.34f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3730:                                          // coruu
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_CORUU_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_CORUU_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_CORUU_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_CORUU_C_CONSOLE, 2426.77f, 2750.38f, 133.24f, 2.14f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3734:                                          // duro
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_DURO_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_DURO_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_DURO_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_DURO_C_CONSOLE, 2976.48f, 2183.29f, 163.20f, 1.85f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
            break;
        case 3722:                                          // ara
            if ((pPlayer->GetQuestStatus(QUEST_SHUTDOWN_ARA_ALDOR) == QUEST_STATUS_INCOMPLETE
                    || pPlayer->GetQuestStatus(QUEST_SHUTDOWN_ARA_SCRYERS) == QUEST_STATUS_INCOMPLETE)
                    && pPlayer->HasItemCount(ITEM_ARA_ACESS_CRYSTAL, 1))
                pManaforge = pPlayer->SummonCreature(NPC_ARA_C_CONSOLE, 4013.71f, 4028.76f, 192.10f, 1.25f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 125000);
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
                m_creature->SummonCreature(NPC_PATHALEON_THE_CALCULATOR_IMAGE, 2325.851563f, 2799.534668f, 133.084229f, 6.038996f, TEMPSPAWN_TIMED_DESPAWN, 90000);
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

UnitAI* GetAI_npc_commander_dawnforge(Creature* pCreature)
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
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.67f, 2183.11f, 96.85f, 6.20f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.53f, 2184.43f, 96.36f, 6.27f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_TORMENTED_SOUL, 2449.85f, 2186.34f, 97.57f, 6.08f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                break;
            case 7:
                m_creature->SummonCreature(NPC_SEVERED_SPIRIT, 2309.64f, 2186.24f, 92.25f, 6.06f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_SEVERED_SPIRIT, 2309.25f, 2183.46f, 91.75f, 6.22f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                break;
            case 12:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_COWS_COME_HOME, m_creature);
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

UnitAI* GetAI_npc_bessy(Creature* pCreature)
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
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_MARK_V_IS_ALIVE, m_creature);

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

UnitAI* GetAI_npc_maxx_a_million(Creature* pCreature)
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
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_PLAYER | TEMPFACTION_TOGGLE_IMMUNE_TO_NPC);
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
                m_creature->CastSpell(m_creature, SPELL_GATHER_WARP_BLOOD, TRIGGERED_NONE);
            }
        }
    }
};

UnitAI* GetAI_npc_zeppit(Creature* pCreature)
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
            pSummoned->CastSpell(pSummoned, SPELL_ETHEREAL_TELEPORT, TRIGGERED_OLD_TRIGGERED);

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
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_DELIVERING_MESSAGE, m_creature);
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
                        m_creature->SummonCreature(NPC_ARCHON, 3875.69f, 2308.72f, 115.80f, 1.48f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                        m_uiEventTimer = 4000;
                        break;
                    case 1:
                        m_creature->SummonCreature(NPC_NEXUS_STALKER, 3884.06f, 2325.22f, 111.37f, 3.45f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                        m_creature->SummonCreature(NPC_NEXUS_STALKER, 3861.54f, 2320.44f, 111.48f, 0.32f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
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

UnitAI* GetAI_npc_protectorate_demolitionist(Creature* pCreature)
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
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_ESCAPE_STAGING_GROUNDS, m_creature);
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

UnitAI* GetAI_npc_captured_vanguard(Creature* pCreature)
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

    SPELL_EXPLOSION_VISUAL  = 30934,                        // Original spell 42458 (Doesn't exist in TBC)

    NPC_EXPLODE_TRIGGER     = 20296,
    NPC_TERROR_IMP          = 20399,
    NPC_LEGION_TROOPER      = 20402,
    NPC_LEGION_DESTROYER    = 20403,

    // GO_ROCKET_SMOKE         = 183988,
    // GO_ROCKET_FIRE          = 183987,

    QUEST_ID_WARP_GATE      = 10310,

    MAX_TROOPERS            = 3,
    MAX_IMPS                = 6,
};

struct npc_drijyaAI : public npc_escortAI
{
    npc_drijyaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_instance = (ScriptedInstance*) m_creature->GetInstanceData();
        m_instance->GetGameObjectGuidVectorFromStorage(GO_ROCKET_SMOKE, m_uiSmokeGuids);
        m_instance->GetGameObjectGuidVectorFromStorage(GO_ROCKET_FIRE, m_uiFireGuids);
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    bool m_uiSayCount;
    uint8 m_uiSpawnCount;
    uint32 m_uiSpawnImpTimer;
    uint32 m_uiSpawnTrooperTimer;
    uint32 m_uiSpawnDestroyerTimer;
    uint32 m_uiDestroyingTimer;

    ObjectGuid m_explodeTriggerGuid;

    ScriptedInstance* m_instance;

    GuidVector m_uiSmokeGuids;
    GuidVector m_uiFireGuids;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSpawnCount          = 0;
            m_uiSpawnImpTimer       = 0;
            m_uiSpawnTrooperTimer   = 0;
            m_uiSpawnDestroyerTimer = 0;
            m_uiDestroyingTimer     = 0;
            m_uiSayCount            = false;
        }
    }

    void AttackedBy(Unit* pWho) override
    {
        if (!m_uiSayCount)
        {
            DoScriptText(SAY_DRIJYA_3, m_creature);
            m_uiSayCount = true;
        }
    }

    void DoSpawnCreature(uint32 uiEntry)
    {
        if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_explodeTriggerGuid))
            m_creature->SummonCreature(uiEntry, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), pTrigger->GetOrientation(), TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
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

    void RespawnGo(ObjectGuid& guid)
    {
        if (GameObject* go = m_creature->GetMap()->GetGameObject(guid))
        {
            go->SetLootState(GO_READY);
            go->SetRespawnTime(300);
            go->Refresh();
        }
    }

    void RespawnGo(bool smoke, uint32 posStart, uint32 posEnd)
    {
        if (smoke)
        {
            auto iter = std::next(m_uiSmokeGuids.begin(), posStart);
            for (uint32 i = posEnd - posStart + 1; iter != m_uiSmokeGuids.end() && i > 0; ++iter, i--)
                RespawnGo((*iter));
        }
        else
        {
            auto iter = std::next(m_uiFireGuids.begin(), posStart);
            for (uint32 i = posEnd - posStart + 1; iter != m_uiFireGuids.end() && i > 0; ++iter, i--)
                RespawnGo((*iter));
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
                m_uiSayCount = false;
                m_uiDestroyingTimer = 60000;
                m_uiSpawnImpTimer = 15000;
                m_uiSpawnCount = 0;
                m_creature->HandleEmote(EMOTE_STATE_WORK);
                if (Creature* pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_EXPLODE_TRIGGER, 40.0f))
                    m_explodeTriggerGuid = pTrigger->GetObjectGuid();
                break;
            case 8:
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                // first pillar smoke
                RespawnGo(true, 0, 0);
                DoScriptText(SAY_DRIJYA_4, m_creature);
                m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
                break;
            case 12:
                SetEscortPaused(true);
                m_uiSayCount = false;
                m_uiDestroyingTimer = 60000;
                m_uiSpawnTrooperTimer = 15000;
                m_uiSpawnCount = 0;
                m_creature->HandleEmote(EMOTE_STATE_WORK);
                break;
            case 13:
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                // second pillar smoke
                RespawnGo(true, 1, 1);
                DoScriptText(SAY_DRIJYA_5, m_creature);
                m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
                break;
            case 17:
                SetEscortPaused(true);
                m_uiSayCount = false;
                m_uiDestroyingTimer = 60000;
                m_uiSpawnDestroyerTimer = 15000;
                m_creature->HandleEmote(EMOTE_STATE_WORK);
                break;
            case 18:
                m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
                // manifold smoke
                RespawnGo(true, 2, 4);
                DoScriptText(SAY_DRIJYA_6, m_creature);
                m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                break;
            case 19:
                // warp gate explosion, smoke and fire
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_explodeTriggerGuid))
                    pTrigger->CastSpell(pTrigger, SPELL_EXPLOSION_VISUAL, TRIGGERED_NONE);
                RespawnGo(false, 0, 4);
                RespawnGo(true, 5, 10);
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
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_WARP_GATE, m_creature);
                }
                m_creature->ClearTemporaryFaction();
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_PLAYER | TEMPFACTION_TOGGLE_IMMUNE_TO_NPC);
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
                    m_uiSpawnTrooperTimer = 6000;
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
                m_uiDestroyingTimer = 0;
            }
            else
                m_uiDestroyingTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

UnitAI* GetAI_npc_drijya(Creature* pCreature)
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
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPAWN_OF_DIMENSIUS)
            pSummoned->CastSpell(m_creature, SPELL_DIMENSIUS_FEEDING, TRIGGERED_OLD_TRIGGERED);
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

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // event is sent by dbscript
        if (eventType == AI_EVENT_CUSTOM_EVENTAI_B && pSender->GetEntry() == NPC_CAPTAIN_SAEED)
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
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
                m_creature->SummonCreature(NPC_SPAWN_OF_DIMENSIUS, fX, fY, fZ, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000);
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

UnitAI* GetAI_npc_dimensius(Creature* pCreature)
{
    return new npc_dimensiusAI(pCreature);
}

enum
{
    NPC_ENERGY_BALL                 = 20769,
    NPC_SALHADAAR                   = 20454,

    SPELL_FLOAT                     = 37613, // actual name C_C_O
    // SPELL_SALAADIN_STASIS           = 35514, purpose unknown
    SPELL_SALAADIN_TESLA            = 35515,
    SPELL_PROTECTORATE_DISRUPTOR    = 35683,
    SPELL_SALAADIN_OVERSPARK        = 35684,
    SPELL_GRAVITY_FLUX              = 36533,
    SPELL_STASIS                    = 36527,
    SPELL_MIRROR_IMAGE_1            = 36847,
    SPELL_MIRROR_IMAGE_2            = 36848,

    FACTION_SALHADAAR_HOSTILE       = 14,

    SAY_THREAT                      = -1000138,
};

struct npc_salhadaarAI : public ScriptedAI
{
    npc_salhadaarAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAttackTimer;
    bool m_uiFoundBalls;
    uint32 m_uiGravityFlux;
    bool m_uiUsedMirrorImage;
    uint32 m_uiStasisTimer;
    std::vector<ObjectGuid> m_uiSummoned;

    void Reset() override
    {
        m_uiAttackTimer = 0;
        m_uiGravityFlux = 15000;
        m_uiUsedMirrorImage = true;
        m_uiStasisTimer = 10000;
        m_creature->RemoveAurasDueToSpell(SPELL_SALAADIN_OVERSPARK);
        m_creature->CastSpell(m_creature, SPELL_FLOAT, TRIGGERED_NONE);
        for (const auto& summonedCreature : m_uiSummoned)    // despawn all summoned creatures
            if (Creature* summon = m_creature->GetMap()->GetCreature(summonedCreature))
                summon->ForcedDespawn(0);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && (!m_creature->isInCombat()) && (!m_creature->isDead()))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_FLOAT);
            DoScriptText(SAY_THREAT, m_creature);
            m_uiAttackTimer = 5000;
        }
    }

    void JustDied(Unit* killer) override
    {
        for (const auto& summonedCreature : m_uiSummoned)    // despawn all summoned creatures
            if (Creature* summon = m_creature->GetMap()->GetCreature(summonedCreature))
                summon->ForcedDespawn(0);
    }

    void JustReachedHome() override
    {
        m_creature->ForcedDespawn(); // despawn at return home
    }

    void JustRespawned() override
    {
        m_uiFoundBalls = false;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_uiSummoned.push_back(pSummoned->GetObjectGuid());
        pSummoned->CastSpell(pSummoned, SPELL_SALAADIN_OVERSPARK, TRIGGERED_OLD_TRIGGERED);
        pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_uiFoundBalls)
        {
            std::list<Creature*> creatureList;
            GetCreatureListWithEntryInGrid(creatureList, m_creature, NPC_ENERGY_BALL, 50.0f);
            for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
            {
                m_uiFoundBalls = true;
                m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, (*itr));
            }
            m_creature->SetSheath(SHEATH_STATE_UNARMED);
        }
        if (m_uiAttackTimer)
        {
            if (m_uiAttackTimer <= uiDiff) // on attack start cast spell that handles everything
            {
                m_uiAttackTimer = 0;
                std::list<Player*> playerList;
                GetPlayerListWithEntryInWorld(playerList, m_creature, 100.0f);
                if (!playerList.empty())
                {
                    AttackStart(playerList.front());
                    m_creature->CastSpell(m_creature, SPELL_SALAADIN_OVERSPARK, TRIGGERED_OLD_TRIGGERED);
                    m_creature->SetFactionTemporary(FACTION_SALHADAAR_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_TOGGLE_NOT_SELECTABLE);
                    return;
                }
                Reset();
                m_creature->ForcedDespawn();
                return;
            }
            m_uiAttackTimer -= uiDiff;
        }
        if (m_creature->isInCombat())
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (!m_uiUsedMirrorImage && m_creature->GetHealthPercent() < 25) // at 25% hp cast mirror image, its two spells
            {
                m_creature->CastSpell(m_creature, SPELL_MIRROR_IMAGE_1, TRIGGERED_OLD_TRIGGERED);
                m_creature->CastSpell(m_creature, SPELL_MIRROR_IMAGE_2, TRIGGERED_OLD_TRIGGERED);
                m_uiUsedMirrorImage = true;
            }
            else
            {
                if (m_uiGravityFlux <= uiDiff)
                {
                    m_uiGravityFlux = 15000;
                    m_creature->CastSpell(m_creature, SPELL_GRAVITY_FLUX, TRIGGERED_NONE); // cast gravity flux every 15 seconds
                    return;
                }
                m_uiGravityFlux -= uiDiff;

                if (m_uiStasisTimer <= uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STASIS))
                    {
                        if (m_creature->CastSpell(target, SPELL_STASIS, TRIGGERED_NONE) == SPELL_CAST_OK) // cast stasis on random target every 22 seconds
                            m_uiStasisTimer = 22000;
                    }
                }
                else
                    m_uiStasisTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

UnitAI* GetAI_npc_salhadaar(Creature* pCreature)
{
    return new npc_salhadaarAI(pCreature);
}

struct npc_energy_ballAI : public ScriptedAI
{
    npc_energy_ballAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiCastTimer;
    uint32 m_uiInterruptTimer;

    void Reset() override
    {
        m_uiCastTimer = 0;
        m_uiInterruptTimer = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_creature->CastSpell(nullptr, SPELL_SALAADIN_TESLA, TRIGGERED_NONE);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_PROTECTORATE_DISRUPTOR) // recast beam after one minute if player doesnt engage mob
        {
            m_uiInterruptTimer = 5000;
            m_uiCastTimer = 65000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiCastTimer)
        {
            if (m_uiCastTimer <= uiDiff)
            {
                m_uiCastTimer = 0;
                std::list<Creature*> creatureList;
                GetCreatureListWithEntryInGrid(creatureList, m_creature, NPC_SALHADAAR, 100.0f);
                for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                {
                    if (!(*itr)->isInCombat())
                    {
                        m_creature->CastSpell(m_creature, SPELL_SALAADIN_TESLA, TRIGGERED_NONE);
                    }
                }
            }
            else
                m_uiCastTimer -= uiDiff;
        }
        if (m_uiInterruptTimer)
        {
            if (m_uiInterruptTimer <= uiDiff)
            {
                m_uiInterruptTimer = 0;
                m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL, false);
            }
            else
                m_uiInterruptTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_energy_ball(Creature* pCreature)
{
    return new npc_energy_ballAI(pCreature);
}

enum
{
    NPC_NEGATRON = 19851,

    SPELL_SCRAP_REAVER = 34630,

    SAY_ON_DEATH = -1000472,

    AREA_PROVING_GROUNDS = 3873,
};

struct npc_scrap_reaverAI : ScriptedPetAI
{
    npc_scrap_reaverAI(Creature* creature) : ScriptedPetAI(creature)
    {
        m_dontDoAnything = false;
        m_despawnTimer = 0;
        m_scriptTimer = 0;
        m_areaCheckTimer = 1000;
    }

    ObjectGuid m_negatron;

    uint32 m_despawnTimer;
    uint32 m_scriptTimer;
    uint32 m_areaCheckTimer;

    bool m_dontDoAnything;

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && !m_dontDoAnything)
            Die();
    }

    void Die()
    {
        if (!m_dontDoAnything)
        {
            SetReactState(REACT_PASSIVE);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->AttackStop();
        }
        m_dontDoAnything = true;
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        if (m_negatron)
            if (Creature* negatron = m_creature->GetMap()->GetCreature(m_negatron))
                DoScriptText(SAY_ON_DEATH, m_creature, negatron);

        m_despawnTimer = 5000;
    }

    void KilledUnit(Unit* victim) override
    {
        HandleKill(victim);
    }

    void OwnerKilledUnit(Unit* victim) override
    {
        HandleKill(victim);
    }

    void HandleKill(Unit* victim)
    {
        if (victim->GetEntry() == NPC_NEGATRON)
        {
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_dontDoAnything = true;
            m_negatron = victim->GetObjectGuid();
            m_scriptTimer = 10000;
            SetReactState(REACT_PASSIVE);
        }
    }

    void UpdateTimers(const uint32 diff)
    {
        if (m_despawnTimer)
        {
            if (m_despawnTimer <= diff)
            {
                m_despawnTimer = 0;
                m_creature->RemoveAurasDueToSpell(SPELL_SCRAP_REAVER);

                if (m_negatron)
                    if (Creature* negatron = m_creature->GetMap()->GetCreature(m_negatron))
                        negatron->ForcedDespawn();

                m_creature->ForcedDespawn();
            }
            else
                m_despawnTimer -= diff;
        }

        if (m_scriptTimer)
        {
            if (m_scriptTimer <= diff)
            {
                m_scriptTimer = 0;
                Die();
            }
            else
                m_scriptTimer -= diff;
        }

        if (m_areaCheckTimer)
        {
            if (m_areaCheckTimer <= diff)
            {
                m_areaCheckTimer = 1000;
                if (m_creature->GetAreaId() != AREA_PROVING_GROUNDS)
                    Die();
            }
            else
                m_areaCheckTimer -= diff;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            return;

        if (m_dontDoAnything)
        {
            UpdateTimers(diff);
            return;
        }

        if (!m_creature->isAlive())                             // should not be needed, isAlive is checked in mangos before calling UpdateAI
            return;

        // UpdateAllies() is done in the generic PetAI in Mangos, but we can't do this from script side.
        // Unclear what side effects this has, but is something to be resolved from Mangos.

        if (m_creature->getVictim())                            // in combat
        {
            if (!m_creature->CanAttack(m_creature->getVictim()))
            {
                // target no longer valid for pet, so either attack stops or new target are selected
                // doesn't normally reach this, because of how petAi is designed in Mangos. CombatStop
                // are called before this update diff, and then pet will already have no victim.
                ResetPetCombat();
                return;
            }

            // update when in combat
            UpdateTimers(diff);
            DoMeleeAttackIfReady();
        }
        else if (m_creature->GetCharmInfo())
        {
            Unit* owner = m_creature->GetMaster();

            if (!owner)
                return;

            if (owner->isInCombat() && !HasReactState(REACT_PASSIVE))
            {
                // Not correct in all cases.
                // When mob initiate attack by spell, pet should not start attack before spell landed.
                AttackStart(owner->getAttackerForHelper());
            }
            else if (m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
            {
                // not following, so start follow
                if (!m_creature->hasUnitState(UNIT_STAT_FOLLOW))
                    m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

                // update when not in combat
                UpdateTimers(diff);
            }
        }
    }
};

UnitAI* GetAI_npc_scrap_reaver(Creature* creature)
{
    return new npc_scrap_reaverAI(creature);
}

enum
{
    SPELL_FEL_ZAPPER            = 35282,

    NPC_ZAXXIS_AMBUSHER         = 20287,

    SAY_EVENT_START             = -1000473,

    COUNT_SPAWNING_LOCATIONS    = 4,
};

float spawningLocations[COUNT_SPAWNING_LOCATIONS][4] =
{
    { 2593.983f, 3946.436f, 137.4138f, 2.598825f },
    { 2543.813f, 3932.396f, 135.5509f, 1.761495f },
    { 2467.507f, 3961.973f, 114.759f, 0.7313454f },
    { 2484.169f, 4042.952f, 131.2856f, 4.728844f },
};

// Used in DB with PathId
float movementDestinations[COUNT_SPAWNING_LOCATIONS * 2][3] =
{

    { 2556.563f, 3969.015f, 131.099f }, // mid point for first
    { 2535.538f, 3992.952f, 137.4138f}, // final point for first

    { 2538.997f, 3957.29f, 128.0989f }, // mid point for second
    { 2527.418f, 3972.693f, 127.7893f }, // final point for second

    { 2496.478f, 3987.993f, 129.061f }, // first point for third
    { 2510.035f, 3995.931f, 132.1609f }, // final point for third

    { 2494.564f, 4020.812f, 133.1337f }, // first point for fourth - guesswork
    { 2518.717f, 4008.459f, 133.8864f }, // final point for fourth - guesswork

};

struct npc_scrapped_fel_reaverAI : ScriptedAI
{
    npc_scrapped_fel_reaverAI(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        m_spawnsPerLocation.resize(COUNT_SPAWNING_LOCATIONS);
        SetReactState(REACT_PASSIVE);
        ResetEvent();
    }

    bool m_eventStarted;
    GuidVector m_spawnsPerLocation;
    int32 m_spawnTimer; // period is exactly 20 seconds with no time lost
    uint8 m_locationCounter;
    int32 m_eventTimer;
    uint8 m_phaseCounter;

    void Reset() override
    {

    }

    void ResetEvent() // has to be separate because its not reset during combat etc
    {
        m_spawnTimer = 0;
        m_locationCounter = 0;
        m_eventTimer = 0;
        m_phaseCounter = 0;
        m_eventStarted = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetActiveObjectState(false);
    }

    void JustRespawned() override
    {
        ResetEvent();
        ScriptedAI::JustRespawned();
    }

    void JustDied(Unit* killer) override
    {
        for (uint32 i = 0; i < COUNT_SPAWNING_LOCATIONS; i++)
        {
            if (Creature* spawn = m_creature->GetMap()->GetCreature(m_spawnsPerLocation[i]))
                spawn->ForcedDespawn();

            m_spawnsPerLocation[i] = ObjectGuid();
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_FEL_ZAPPER)
        {
            m_creature->SetActiveObjectState(true);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_eventStarted = true;
            DoScriptText(SAY_EVENT_START, m_creature, caster);
            m_eventTimer = 240 * IN_MILLISECONDS;
            ProcessEvent();
        }
    }

    void ClearSpawn(Creature* summoned)
    {
        for (uint32 i = 0; i < COUNT_SPAWNING_LOCATIONS; i++)
        {
            if (m_spawnsPerLocation[i] == summoned->GetObjectGuid())
            {
                m_spawnsPerLocation[i] = ObjectGuid();
                break;
            }
        }
    }

    // Does not move in any way on evade TODO: Generically
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        m_creature->SetLootRecipient(nullptr);

        m_creature->TriggerEvadeEvents();
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType == WAYPOINT_MOTION_TYPE && data == 2 && m_creature->getVictim())
            summoned->AI()->AttackStart(m_creature->getVictim());
    }

    void JustSummoned(Creature* summoned) override
    {
        m_spawnsPerLocation[m_locationCounter] = summoned->GetObjectGuid();
        m_locationCounter = (m_locationCounter + 1) % COUNT_SPAWNING_LOCATIONS;
    }

    // new spawn location can happen after one dies
    void SummonedCreatureJustDied(Creature* summoned) override
    {
        ClearSpawn(summoned);
    }

    // has to be done in both, because spawns can despawn before event ends (blizzlike) without dying and then respawn
    void SummonedCreatureDespawn(Creature* summoned) override
    {
        ClearSpawn(summoned);
    }

    void ProcessEvent()
    {
        if (m_spawnTimer <= 0)
        {
            m_spawnTimer += 20 * IN_MILLISECONDS;
            uint32 i;
            // TODO: Sometimes possible to spawn two at once?
            for (i = 0; i < COUNT_SPAWNING_LOCATIONS; i++, m_locationCounter = (m_locationCounter + 1) % COUNT_SPAWNING_LOCATIONS)
                if (!m_spawnsPerLocation[m_locationCounter])
                    break;

            if (i != COUNT_SPAWNING_LOCATIONS)
                m_creature->SummonCreature(NPC_ZAXXIS_AMBUSHER, spawningLocations[m_locationCounter][0], spawningLocations[m_locationCounter][1], spawningLocations[m_locationCounter][2], spawningLocations[m_locationCounter][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 240000, true, true, m_locationCounter + 1);
        }
        if (m_eventTimer <= 0)
        {
            switch (m_phaseCounter)
            {
                case 0: // stop spawning
                    m_phaseCounter++;
                    m_eventTimer = 100 * IN_MILLISECONDS;
                    break;
                case 1: // revert to unattackable when out of combat
                    if (!m_creature->isInCombat())
                        ResetEvent();
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_eventStarted)
        {
            m_spawnTimer -= diff;
            if (m_eventTimer > 0)
                m_eventTimer -= diff;
            ProcessEvent();
        }
    }
};

UnitAI* GetAI_npc_scrapped_fel_reaver(Creature* creature)
{
    return new npc_scrapped_fel_reaverAI(creature);
}

/*
## Deathblow to the Legion script
*/

enum
{
    QUEST_DEATHBLOW_TO_THE_LEGION = 10409,

    FACTION_DEATHBLOW               = 250, // during quest they have different faction
    FACTION_DEATHBLOW_IN_PROGRESS   = 495,
    FACTION_SHATTRATH               = 1743,

    FACTION_KAYLAAN_HOSTILE         = 14,
    FACTION_KAYLAAN_REDEEMED        = 290,

    NPC_ADYEN_THE_LIGHTWARDEN   = 18537,
    NPC_EXARCH_ORELIS           = 19466,
    NPC_ANCHORITE_KARJA         = 19467,
    NPC_ISHANAH                 = 18538,
    NPC_KAYLAAN_THE_LOST        = 20794,
    NPC_WRATH_LORD              = 20929,
    // NPC_SOCRETHAR              = 20132  // defined in world map script

    GOSSIP_SHATTRATH                        = 8117,
    GOSSIP_NETHERSTORM                      = 8215,
    GOSSIP_SOCRETHAR_DEAD_PLACEHOLDER_ID    = 40000, // uses a placeholder id due to no sniff available
    GOSSIP_ACTION_START_DEATHBLOW = 1,

    SPELL_POWER_OF_THE_LEGION           = 35596,
    SPELL_KAYLAN_WRATH                  = 35614,
    SPELL_CANCEL_POWER_OF_THE_LEGION    = 35597,
    SPELL_WRATH_OF_SOCRETHAR            = 35598, // Ishanah version
    SPELL_RESURRECTION                  = 35599,
    SPELL_WRATH_OF_SOCRETHAR2           = 35600, // Kaylaan version
    SPELL_DIVINE_SHIELD                 = 13874,
    SPELL_PERMANENT_FEIGN_DEATH         = 29266,

    // combat spells
    // Adyen
    SPELL_CRUSADER_STRIKE   = 14518,
    SPELL_HAMMER_OF_JUSTICE = 13005,

    // Kaylaan
    SPELL_BURNING_LIGHT     = 37552,
    SPELL_HOLY_SLAM         = 37572,
    SPELL_HEAL              = 37569,
    SPELL_AVENGERS_SHEILD   = 37554,

    // Ishanah
    SPELL_HOLY_SMITE        = 15238,
    SPELL_POWER_WORD_SHIELD = 22187,
    SPELL_GREATER_HEAL      = 35096,

    // Anchorite Karja
    SPELL_HOLY_FIRE = 17141,

    // Exarch Orelis
    SPELL_DEMORALIZING_SHOUT    = 13730,
    SPELL_REND                  = 16509,
    SPELL_HEROIC_STRIKE         = 29426,

    // Socrethar
    SPELL_CLEAVE                = 15496,
    SPELL_SHADOW_BOLT_VOLLEY    = 28448,
    SPELL_FIREBALL_BARRAGE      = 37540,

    SAY_ADYEN_1     = -1001254,
    SAY_SOCRETHAR_1 = -1001255,
    SAY_ADYEN_2     = -1001256,
    SAY_SOCRETHAR_2 = -1001257,
    SAY_ORELIS_1    = -1001258,
    SAY_KAYLAAN_1   = -1001259,
    SAY_KAYLAAN_2   = -1001260,
    SAY_KAYLAAN_3   = -1001261,
    SAY_ADYEN_3     = -1001262,
    SAY_KAYLAAN_4   = -1001263,
    SAY_SOCRETHAR_3 = -1001264,
    SAY_SOCRETHAR_4 = -1001265,
    SAY_KAYLAAN_5   = -1001266,
    SAY_KAYLAAN_6   = -1001267,
    SAY_ISHANAH_1   = -1001268,
    SAY_ISHANAH_2   = -1001269,
    SAY_SOCRETHAR_5 = -1001270,
    SAY_KAYLAAN_7   = -1001271,
    SAY_KAYLAAN_8   = -1001272,
    SAY_SOCRETHAR_6 = -1001273,

    EVENT_SOCRETHAR_SAY_1   = 0,
    EVENT_ADYEN_SAY_2       = 1,
    EVENT_SOCRETHAR_SAY_2   = 2,
    EVENT_KAYLAAN_MOVE      = 3,
    EVENT_KAYLAAN_SAY_2     = 4,
    EVENT_KAYLAAN_SAY_3     = 5,
    EVENT_ADYEN_SAY_3       = 6,
    EVENT_KAYLAAN_SAY_4     = 7,
    EVENT_SOCRETHAR_CAST_POWER_OF_LEGION    = 8,
    EVENT_SOCRETHAR_SAY_3                   = 9,
    EVENT_KAYLAAN_SAY_5                     = 10,
    EVENT_KAYLAAN_KNEEL                     = 11,
    EVENT_ISHANAH_SAY_1                     = 12,
    EVENT_ISHANAH_SAY_2                     = 13,
    EVENT_SOCRETHAR_SAY_5                   = 14,
    EVENT_SOCRETHAR_KILL_ISHANAH            = 15,
    EVENT_KAYLAAN_HORRIFIED                 = 16,
    EVENT_KAYLAAN_SAVE_ISHANAH              = 17,
    EVENT_SOCRETHAR_SAY_6                   = 18,
    EVENT_SOCRETHAR_KILL_KAYLAAN            = 19,
    EVENT_SOCRETHAR_ATTACK                  = 20,

    POINT_KAYLAAN_MOVE = 1,
    POINT_KAYLAAN_SAVE_ISHANAH = 2,

    PATH_ID_DEATHBLOW = 1,
    WAYPOINT_ADYEN_LAST = 8,

    DB_GUID_WRATH_LORD_1 = 73683,
    DB_GUID_WRATH_LORD_2 = 73693,

    PATH_ID_WRATH_LORD_1 = 1,
    PATH_ID_WRATH_LORD_2 = 2,
};

struct npc_adyen_the_lightwardenAI : public ScriptedAI
{
    npc_adyen_the_lightwardenAI(Creature* creature) : ScriptedAI(creature), m_crusaderStrikeTimer(0), m_hammerOfJusticeTimer(0), m_eventStarted(false)
    {
    }

    ObjectGuid m_playerGuid;
    ObjectGuid m_orelisGuid;
    ObjectGuid m_karjaGuid;
    ObjectGuid m_ishanahGuid;
    ObjectGuid m_kaylaanGuid;
    ObjectGuid m_socretharGuid;

    uint32 m_crusaderStrikeTimer;
    uint32 m_hammerOfJusticeTimer;

    bool m_eventStarted;

    std::map<uint32, uint32> m_timers;

    void Reset() override
    {
        m_crusaderStrikeTimer = 10000;
        m_hammerOfJusticeTimer = 20000;
    }

    void AddTimer(const uint32 id, const uint32 timer)
    {
        m_timers[id] = timer;
    }

    void UpdateTimers(const uint32 diff)
    {
        std::vector<uint32> removedTimers;

        for (auto& m_timer : m_timers)
        {
            if (m_timer.second <= diff)
            {
                ExecuteTimer(m_timer.first);
                removedTimers.push_back(m_timer.first);
            }
            else
                m_timer.second -= diff;
        }

        for (uint32 id : removedTimers)
            m_timers.erase(id);
    }

    void ExecuteTimer(const uint32 id)
    {
        switch (id)
        {
            case EVENT_SOCRETHAR_SAY_1:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    DoScriptText(SAY_SOCRETHAR_1, socrethar, GetPlayerTarget());
                AddTimer(EVENT_ADYEN_SAY_2, 6500);
                break;
            case EVENT_ADYEN_SAY_2:
                DoScriptText(SAY_ADYEN_2, m_creature, GetPlayerTarget());
                AddTimer(EVENT_SOCRETHAR_SAY_2, 6500);
                m_creature->SummonCreature(NPC_KAYLAAN_THE_LOST, 4955.793f, 3926.349f, 208.391f, 4.570264f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3600000, true, false, 1);
                break;
            case EVENT_SOCRETHAR_SAY_2:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    DoScriptText(SAY_SOCRETHAR_2, socrethar, GetPlayerTarget());
                break;
            case EVENT_KAYLAAN_MOVE:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                {
                    kaylaan->SetStandState(UNIT_STAND_STATE_STAND);
                    kaylaan->GetMotionMaster()->MovePoint(POINT_KAYLAAN_MOVE, 4940.221f, 3847.434f, 211.5027f);
                }
                break;
            case EVENT_KAYLAAN_SAY_2:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                {
                    DoScriptText(SAY_KAYLAAN_2, kaylaan, GetPlayerTarget());
                    AddTimer(EVENT_KAYLAAN_SAY_3, 9000);
                }
                break;
            case EVENT_KAYLAAN_SAY_3:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                    DoScriptText(SAY_KAYLAAN_3, kaylaan, GetPlayerTarget());
                AddTimer(EVENT_ADYEN_SAY_3, 6000);
                break;
            case EVENT_ADYEN_SAY_3:
                DoScriptText(SAY_ADYEN_3, m_creature, GetPlayerTarget());
                AddTimer(EVENT_KAYLAAN_SAY_4, 7000);
                break;
            case EVENT_KAYLAAN_SAY_4:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                    DoScriptText(SAY_KAYLAAN_4, kaylaan, GetPlayerTarget());
                AddTimer(EVENT_SOCRETHAR_CAST_POWER_OF_LEGION, 6000);
                break;
            case EVENT_SOCRETHAR_CAST_POWER_OF_LEGION:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    socrethar->CastSpell(nullptr, SPELL_POWER_OF_THE_LEGION, TRIGGERED_NONE);

                AddTimer(EVENT_SOCRETHAR_SAY_3, 3500);
                break;
            case EVENT_SOCRETHAR_SAY_3:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    DoScriptText(SAY_SOCRETHAR_3, socrethar, GetPlayerTarget());

                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                {
                    kaylaan->setFaction(FACTION_KAYLAAN_HOSTILE);
                    if (Player* player = GetPlayerTarget())
                        kaylaan->AI()->AttackStart(player);
                    else
                        kaylaan->AI()->AttackStart(m_creature);
                    AttackStart(kaylaan);
                    if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
                        orelis->AI()->AttackStart(kaylaan);
                    if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
                        karja->AI()->AttackStart(kaylaan);
                }
                break;
            case EVENT_KAYLAAN_SAY_5:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                    DoScriptText(SAY_KAYLAAN_5, kaylaan, GetPlayerTarget());
                break;
            case EVENT_KAYLAAN_KNEEL:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                    kaylaan->SetStandState(UNIT_STAND_STATE_KNEEL);
                AddTimer(EVENT_ISHANAH_SAY_1, 500);
                break;
            case EVENT_ISHANAH_SAY_1:
                if (Creature* ishanah = m_creature->GetMap()->GetCreature(m_ishanahGuid))
                    DoScriptText(SAY_ISHANAH_1, ishanah, m_creature->GetMap()->GetCreature(m_kaylaanGuid));
                AddTimer(EVENT_ISHANAH_SAY_2, 7000);
                break;
            case EVENT_ISHANAH_SAY_2:
                if (Creature* ishanah = m_creature->GetMap()->GetCreature(m_ishanahGuid))
                    DoScriptText(SAY_ISHANAH_2, ishanah, m_creature->GetMap()->GetCreature(m_kaylaanGuid));
                AddTimer(EVENT_SOCRETHAR_SAY_5, 7500);
                break;
            case EVENT_SOCRETHAR_SAY_5:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    DoScriptText(SAY_SOCRETHAR_5, socrethar, m_creature->GetMap()->GetCreature(m_kaylaanGuid));
                AddTimer(EVENT_SOCRETHAR_KILL_ISHANAH, 3000);
                break;
            case EVENT_SOCRETHAR_KILL_ISHANAH:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    socrethar->CastSpell(nullptr, SPELL_WRATH_OF_SOCRETHAR, TRIGGERED_NONE);
                AddTimer(EVENT_KAYLAAN_HORRIFIED, 7500);
                break;
            case EVENT_KAYLAAN_HORRIFIED:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                {
                    kaylaan->CastSpell(kaylaan, SPELL_CANCEL_POWER_OF_THE_LEGION, TRIGGERED_NONE);
                    DoScriptText(SAY_KAYLAAN_7, kaylaan, kaylaan);
                    kaylaan->setFaction(FACTION_KAYLAAN_REDEEMED);
                    kaylaan->SetStandState(UNIT_STAND_STATE_STAND);
                    kaylaan->GetMotionMaster()->MovePoint(POINT_KAYLAAN_SAVE_ISHANAH, 4941.518f, 3837.293f, 211.5089f);
                }
                break;
            case EVENT_KAYLAAN_SAVE_ISHANAH:
                if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                    kaylaan->CastSpell(nullptr, SPELL_RESURRECTION, TRIGGERED_NONE);
                AddTimer(EVENT_SOCRETHAR_SAY_6, 6000);
                break;
            case EVENT_SOCRETHAR_SAY_6:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    DoScriptText(SAY_SOCRETHAR_6, socrethar, m_creature->GetMap()->GetCreature(m_kaylaanGuid));
                AddTimer(EVENT_SOCRETHAR_KILL_KAYLAAN, 6000);
                break;
            case EVENT_SOCRETHAR_KILL_KAYLAAN:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                    socrethar->CastSpell(nullptr, SPELL_WRATH_OF_SOCRETHAR2, TRIGGERED_NONE);
                AddTimer(EVENT_SOCRETHAR_ATTACK, 3500);
                break;
            case EVENT_SOCRETHAR_ATTACK:
                if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                {
                    socrethar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    socrethar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    if (Creature* ishanah = m_creature->GetMap()->GetCreature(m_ishanahGuid))
                        socrethar->AI()->AttackStart(ishanah);
                    AttackStart(socrethar);
                    if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
                        orelis->AI()->AttackStart(socrethar);
                    if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
                        karja->AI()->AttackStart(socrethar);
                }
                break;
            default:
                break;
        }
    }

    Player* GetPlayerTarget() const
    {
        return m_creature->GetMap()->GetPlayer(m_playerGuid);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_EXARCH_ORELIS:     m_orelisGuid = summoned->GetObjectGuid(); break;
            case NPC_ANCHORITE_KARJA:   m_karjaGuid = summoned->GetObjectGuid(); break;
            case NPC_ISHANAH:           m_ishanahGuid = summoned->GetObjectGuid(); break;
            case NPC_KAYLAAN_THE_LOST:  m_kaylaanGuid = summoned->GetObjectGuid(); break;
            default: break;
        }
    }

    void StartEvent(Player* player)
    {
        if (Creature* socrethar = ((ScriptedInstance*)m_creature->GetMap()->GetInstanceData())->GetSingleCreatureFromStorage(NPC_SOCRETHAR))
        {
            if (!socrethar->isAlive())
                return;

            m_socretharGuid = socrethar->GetObjectGuid();
            socrethar->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        }
        else
            return;

        if (m_eventStarted)
            return;

        m_playerGuid = player->GetObjectGuid();
        m_creature->setFaction(FACTION_DEATHBLOW_IN_PROGRESS);
        // TODO: Formation movement
        if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
        {
            orelis->setFaction(FACTION_DEATHBLOW_IN_PROGRESS);
            orelis->GetMotionMaster()->MoveFollow(m_creature, 1.f, M_PI_F - M_PI_F / 4, true);
        }
        if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
        {
            karja->setFaction(FACTION_DEATHBLOW_IN_PROGRESS);
            karja->GetMotionMaster()->MoveFollow(m_creature, 1.f, M_PI_F + M_PI_F / 4, true);
        }
        if (Creature* wrathLord = m_creature->GetMap()->GetCreature(ObjectGuid(HIGHGUID_UNIT, uint32(NPC_WRATH_LORD), uint32(DB_GUID_WRATH_LORD_1))))
        {
            wrathLord->GetMotionMaster()->Clear(false, true);
            wrathLord->GetMotionMaster()->MoveWaypoint(PATH_ID_WRATH_LORD_1);
        }
        if (Creature* wrathLord = m_creature->GetMap()->GetCreature(ObjectGuid(HIGHGUID_UNIT, uint32(NPC_WRATH_LORD), uint32(DB_GUID_WRATH_LORD_2))))
        {
            wrathLord->GetMotionMaster()->Clear(false, true);
            wrathLord->GetMotionMaster()->MoveWaypoint(PATH_ID_WRATH_LORD_2);
        }

        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_DEATHBLOW);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_eventStarted = true;
    }

    void DespawnEvent()
    {
        if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
            orelis->ForcedDespawn(10000);
        if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
            karja->ForcedDespawn(10000);
        m_creature->ForcedDespawn(10000);
    }

    void FailEvent()
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
            player->FailQuestForGroup(QUEST_DEATHBLOW_TO_THE_LEGION);
        if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
            orelis->ForcedDespawn();
        if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
            karja->ForcedDespawn();
        if (Creature* ishanah = m_creature->GetMap()->GetCreature(m_ishanahGuid))
            ishanah->ForcedDespawn();
        if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
            socrethar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->ForcedDespawn();
    }

    void EndEvent(Creature* ishanah)
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
            player->RewardPlayerAndGroupAtEventExplored(QUEST_DEATHBLOW_TO_THE_LEGION, m_creature);
        if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
            orelis->ForcedDespawn(60000); // can be dead
        if (Creature* karja = m_creature->GetMap()->GetCreature(m_karjaGuid))
            karja->ForcedDespawn(60000); // can be dead
        ishanah->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        ishanah->ForcedDespawn(60000);
        m_creature->ForcedDespawn(60000);
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == WAYPOINT_MOTION_TYPE)
            WaypointReached(data);
    }

    void SummonedMovementInform(Creature* summoned, uint32 movementType, uint32 data) override
    {
        switch (movementType)
        {
            case WAYPOINT_MOTION_TYPE:
                SummonedWaypointReached(summoned, data);
                break;
            case POINT_MOTION_TYPE:
                SummonedPointReached(summoned, data);
                break;
        }
    }

    void WaypointReached(uint32 pointId)
    {
        switch (pointId)
        {
            case WAYPOINT_ADYEN_LAST: // Last initial waypoint
            {
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                DoScriptText(SAY_ADYEN_1, m_creature, m_creature->GetMap()->GetPlayer(m_playerGuid));
                AddTimer(EVENT_SOCRETHAR_SAY_1, 6500);
                break;
            }
            default:
                break;
        }
    }

    void SummonedWaypointReached(Creature* summoned, uint32 pointId)
    {
        switch (summoned->GetEntry())
        {
            case NPC_ISHANAH:
                switch (pointId)
                {
                    case 7:
                        summoned->SetWalk(true);
                        break;
                    case 8:
                        summoned->GetMotionMaster()->Clear(false, true);
                        summoned->GetMotionMaster()->MoveIdle();
                        if (Creature* kaylaan = m_creature->GetMap()->GetCreature(m_kaylaanGuid))
                        {
                            DoScriptText(SAY_KAYLAAN_6, kaylaan, GetPlayerTarget());
                            kaylaan->SetFacingToObject(summoned);
                        }
                        if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                            socrethar->SetFacingToObject(summoned);
                        AddTimer(EVENT_KAYLAAN_KNEEL, 6000);
                        break;
                    default:
                        break;
                }
                break;
            case NPC_KAYLAAN_THE_LOST:
                switch (pointId)
                {
                    case 6:
                        summoned->GetMotionMaster()->Clear(false, true);
                        summoned->GetMotionMaster()->MoveIdle();
                        summoned->SetStandState(UNIT_STAND_STATE_KNEEL);
                        if (Creature* orelis = m_creature->GetMap()->GetCreature(m_orelisGuid))
                            DoScriptText(SAY_ORELIS_1, orelis, GetPlayerTarget());
                        AddTimer(EVENT_KAYLAAN_MOVE, 6500);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    void SummonedPointReached(Creature* summoned, uint32 pointId)
    {
        switch (summoned->GetEntry())
        {
            case NPC_KAYLAAN_THE_LOST:
                switch (pointId)
                {
                    case POINT_KAYLAAN_MOVE:
                        DoScriptText(SAY_KAYLAAN_1, summoned, GetPlayerTarget());
                        summoned->SetFacingTo(3.837011f);
                        AddTimer(EVENT_KAYLAAN_SAY_2, 9000);
                        break;
                    case POINT_KAYLAAN_SAVE_ISHANAH:
                        summoned->CastSpell(summoned, SPELL_DIVINE_SHIELD, TRIGGERED_NONE);
                        DoScriptText(SAY_KAYLAAN_8, summoned, summoned);
                        AddTimer(EVENT_KAYLAAN_SAVE_ISHANAH, 1000);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    void SummonedJustReachedHome(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ISHANAH:
            {
                EndEvent(summoned);
                break;
            }
            case NPC_ANCHORITE_KARJA:
                summoned->GetMotionMaster()->MoveFollow(m_creature, 1.f, M_PI_F + M_PI_F / 4, true);
                break;
            case NPC_EXARCH_ORELIS:
                summoned->GetMotionMaster()->MoveFollow(m_creature, 1.f, M_PI_F - M_PI_F / 4, true);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            Creature* ishanah = m_creature->SummonCreature(NPC_ISHANAH, 4866.2f, 3799.016f, 199.141f, 0.4680258f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3600000, true, true, 1, FACTION_DEATHBLOW);
            ishanah->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            ishanah->GetMotionMaster()->Clear(false, true);
            ishanah->GetMotionMaster()->MoveWaypoint(PATH_ID_DEATHBLOW);
            if (Creature* socrethar = m_creature->GetMap()->GetCreature(m_socretharGuid))
                DoScriptText(SAY_SOCRETHAR_4, socrethar, sender);
            AddTimer(EVENT_KAYLAAN_SAY_5, 7000);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        FailEvent();
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_hammerOfJusticeTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE) == CAST_OK)
            {
                m_hammerOfJusticeTimer = 30000;
                return;
            }
        }
        else
            m_hammerOfJusticeTimer -= diff;

        if (m_crusaderStrikeTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADER_STRIKE) == CAST_OK)
            {
                m_crusaderStrikeTimer = 15000;
                return;
            }
        }
        else
            m_crusaderStrikeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

enum KaylaanActions
{
    KAYLAAN_ACTION_HEAL,
    KAYLAAN_ACTION_AVENGERS,
    KAYLAAN_ACTION_HOLY_SLAM,
    KAYLAAN_ACTION_BURNING_LIGHT,
    KAYLAAN_ACTION_MAX,
};

struct npc_kaylaan_the_lostAI : public ScriptedAI
{
    npc_kaylaan_the_lostAI(Creature* creature) : ScriptedAI(creature), m_deathPrevented(false), m_avengersShieldTimer(0), m_burningLightTimer(0), m_healTimer(0), m_holySlamTimer(0)
    {
    }

    bool m_deathPrevented;

    uint32 m_avengersShieldTimer;
    uint32 m_burningLightTimer;
    uint32 m_healTimer;
    uint32 m_holySlamTimer;

    bool m_actionReadyStatus[KAYLAAN_ACTION_MAX];

    void Reset() override
    {
        m_avengersShieldTimer = 0;
        m_burningLightTimer = 2000;
        m_healTimer = 0;
        m_holySlamTimer = 10000;

        m_actionReadyStatus[KAYLAAN_ACTION_HEAL] = false;
        m_actionReadyStatus[KAYLAAN_ACTION_AVENGERS] = true;
        m_actionReadyStatus[KAYLAAN_ACTION_BURNING_LIGHT] = false;
        m_actionReadyStatus[KAYLAAN_ACTION_HOLY_SLAM] = false;
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell) override
    {
        if (target->GetEntry() == NPC_ISHANAH && spell->Id == SPELL_RESURRECTION)
            target->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
    }

    void DamageTaken(Unit* /*killer*/, uint32& damage, DamageEffectType /*damagetype*/) override
    {
        if (damage < m_creature->GetHealth())
            return;

        damage = 0;

        if (m_deathPrevented)
            return;

        m_deathPrevented = true;

        m_creature->SetHealth(1);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->CastSpell(nullptr, SPELL_KAYLAN_WRATH, TRIGGERED_NONE);
        EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (Unit* spawner = m_creature->GetSpawner()) // spawner is Adyen
            if (spawner->AI() && spawner->GetTypeId() == TYPEID_UNIT)
                m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spawner, static_cast<Creature*>(spawner));
    }

    void UpdateActions()
    {
        if (!m_actionReadyStatus[KAYLAAN_ACTION_HEAL])
            if (m_creature->GetHealthPercent() < 50.f && !m_healTimer)
                m_actionReadyStatus[KAYLAAN_ACTION_HEAL] = true;
    }

    void ExecuteActions()
    {
        if (m_creature->IsNonMeleeSpellCasted(false) || m_combatScriptHappening)
            return;

        for (uint32 i = 0; i < KAYLAAN_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case KAYLAAN_ACTION_HEAL:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_HEAL) == CAST_OK)
                        {
                            m_healTimer = 15000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAYLAAN_ACTION_AVENGERS:
                    {
                        if (m_creature->IsInRange(m_creature->getVictim(), 8.f, 30.f))
                        {
                            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_AVENGERS_SHEILD) == CAST_OK)
                            {
                                m_avengersShieldTimer = 15000;
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case KAYLAAN_ACTION_HOLY_SLAM:
                    {
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_SLAM) == CAST_OK)
                        {
                            m_holySlamTimer = 30000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAYLAAN_ACTION_BURNING_LIGHT:
                    {
                        if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BURNING_LIGHT) == CAST_OK)
                        {
                            m_burningLightTimer = 15000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }


    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_actionReadyStatus[KAYLAAN_ACTION_AVENGERS])
        {
            if (m_avengersShieldTimer <= diff)
            {
                m_avengersShieldTimer = 0;
                m_actionReadyStatus[KAYLAAN_ACTION_AVENGERS] = true;
            }
            else
                m_avengersShieldTimer -= diff;
        }

        if (m_healTimer)
        {
            if (m_healTimer <= diff)
                m_healTimer = 0;
            else
                m_healTimer -= diff;
        }

        if (!m_actionReadyStatus[KAYLAAN_ACTION_HOLY_SLAM])
        {
            if (m_holySlamTimer <= diff)
            {
                m_holySlamTimer = 0;
                m_actionReadyStatus[KAYLAAN_ACTION_HOLY_SLAM] = true;
            }
            else
                m_holySlamTimer -= diff;
        }

        if (!m_actionReadyStatus[KAYLAAN_ACTION_BURNING_LIGHT])
        {
            if (m_burningLightTimer <= diff)
            {
                m_burningLightTimer = 0;
                m_actionReadyStatus[KAYLAAN_ACTION_BURNING_LIGHT] = true;
            }
            else
                m_burningLightTimer -= diff;
        }

        UpdateActions();
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* Getnpc_adyen_the_lightwardenAI(Creature* creature)
{
    return new npc_adyen_the_lightwardenAI(creature);
}

UnitAI* Getnpc_kaylaan_the_lostAI(Creature* creature)
{
    return new npc_kaylaan_the_lostAI(creature);
}

bool AreaTrigger_at_socrethar_seat(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsCurrentQuest(QUEST_DEATHBLOW_TO_THE_LEGION))
    {
        if (player->GetMap()->SpawnedCountForEntry(NPC_ADYEN_THE_LIGHTWARDEN) == 0)
        {
            Creature* adyen = player->SummonCreature(NPC_ADYEN_THE_LIGHTWARDEN, 4805.796f, 3774.112f, 210.6172f, 5.550147f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3600000, true, false, 1, FACTION_DEATHBLOW, 0, true);
            adyen->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            Creature* orelis = adyen->SummonCreature(NPC_EXARCH_ORELIS, 4804.807f, 3775.562f, 210.6171f, 5.532694f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3600000, true, false, 1, FACTION_DEATHBLOW);
            orelis->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            Creature* karja = adyen->SummonCreature(NPC_ANCHORITE_KARJA, 4803.637f, 3773.844f, 210.6176f, 5.602507f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3600000, true, false, 1, FACTION_DEATHBLOW);
            karja->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        }
    }
    return false;
}

bool GossipHello_npc_adyen_the_lightwarden(Player* player, Creature* creature)
{
    uint32 gossipId = GOSSIP_SOCRETHAR_DEAD_PLACEHOLDER_ID;

    // custom code required because it utilizes two entries
    if (creature->getFaction() == FACTION_SHATTRATH)
        gossipId = GOSSIP_SHATTRATH;
    else
    {
        if (npc_adyen_the_lightwardenAI* ai = dynamic_cast<npc_adyen_the_lightwardenAI*>(creature->AI()))
        {
            if (ai->m_eventStarted)
                gossipId = GOSSIP_NETHERSTORM;
            else if (Creature* socrethar = ((ScriptedInstance*)creature->GetMap()->GetInstanceData())->GetSingleCreatureFromStorage(NPC_SOCRETHAR))
            {
                if (socrethar->isAlive())
                    gossipId = GOSSIP_NETHERSTORM;
                else
                    ai->DespawnEvent();
            }
        }
    }
    player->PrepareGossipMenu(creature, gossipId);
    player->SendPreparedGossip(creature);
    return true;
}

bool GossipSelect_npc_adyen_the_lightwarden(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == 1)
    {
        if (npc_adyen_the_lightwardenAI* ai = dynamic_cast<npc_adyen_the_lightwardenAI*>(creature->AI()))
            ai->StartEvent(player);

        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

void AddSC_netherstorm()
{
    Script* pNewScript = new Script;
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

    pNewScript = new Script;
    pNewScript->Name = "npc_salhadaar";
    pNewScript->GetAI = &GetAI_npc_salhadaar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_energy_ball";
    pNewScript->GetAI = &GetAI_npc_energy_ball;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scrap_reaver";
    pNewScript->GetAI = &GetAI_npc_scrap_reaver;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scrapped_fel_reaver";
    pNewScript->GetAI = &GetAI_npc_scrapped_fel_reaver;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_socrethar_seat";
    pNewScript->pAreaTrigger = &AreaTrigger_at_socrethar_seat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_adyen_the_lightwarden";
    pNewScript->GetAI = &Getnpc_adyen_the_lightwardenAI;
    pNewScript->pGossipHello = &GossipHello_npc_adyen_the_lightwarden;
    pNewScript->pGossipSelect = &GossipSelect_npc_adyen_the_lightwarden;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kaylaan_the_lost";
    pNewScript->GetAI = &Getnpc_kaylaan_the_lostAI;
    pNewScript->RegisterSelf();
}
