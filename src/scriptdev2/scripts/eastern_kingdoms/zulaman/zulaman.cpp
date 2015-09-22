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
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

/* ContentData
npc_forest_frog
EndContentData */

#include "precompiled.h"
#include "zulaman.h"
#include "escort_ai.h"

/*######
## npc_forest_frog
######*/

enum
{
    SPELL_REMOVE_AMANI_CURSE = 43732,
    SPELL_PUSH_MOJO          = 43923,
    NPC_FOREST_FROG          = 24396
};

struct npc_forest_frogAI : public ScriptedAI
{
    npc_forest_frogAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void DoSpawnRandom()
    {
        if (m_pInstance)
        {
            uint32 cEntry = 0;
            switch (urand(0, 10))
            {
                case 0: cEntry = 24024; break;              // Kraz      // wrong here?
                case 1: cEntry = 24397; break;              // Mannuth
                case 2: cEntry = 24403; break;              // Deez
                case 3: cEntry = 24404; break;              // Galathryn
                case 4: cEntry = 24405; break;              // Adarrah
                case 5: cEntry = 24406; break;              // Fudgerick
                case 6: cEntry = 24407; break;              // Darwen
                case 7: cEntry = 24445; break;              // Mitzi
                case 8: cEntry = 24448; break;              // Christian
                case 9: cEntry = 24453; break;              // Brennan
                case 10: cEntry = 24455; break;             // Hollee
            }

            if (!m_pInstance->GetData(TYPE_RAND_VENDOR_1))
                if (!urand(0, 9))
                    cEntry = 24408;                         // Gunter

            if (!m_pInstance->GetData(TYPE_RAND_VENDOR_2))
                if (!urand(0, 9))
                    cEntry = 24409;                         // Kyren

            if (cEntry)
                m_creature->UpdateEntry(cEntry);

            if (cEntry == 24408)
                m_pInstance->SetData(TYPE_RAND_VENDOR_1, DONE);

            if (cEntry == 24409)
                m_pInstance->SetData(TYPE_RAND_VENDOR_2, DONE);
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && m_creature->GetEntry() == NPC_FOREST_FROG)
        {
            // increase or decrease chance of mojo?
            if (!urand(0, 49))
                DoCastSpellIfCan(caster, SPELL_PUSH_MOJO, CAST_TRIGGERED);
            else
                DoSpawnRandom();
        }
    }
};
CreatureAI* GetAI_npc_forest_frog(Creature* pCreature)
{
    return new npc_forest_frogAI(pCreature);
}

/*######
## npc_harrison_jones_za
######*/

enum
{
    SAY_START               = -1568079,
    SAY_AT_GONG             = -1568080,
    SAY_OPEN_ENTRANCE       = -1568081,

    GOSSIP_ITEM_ID_BEGIN    = -3568000,

    SPELL_BANGING_THE_GONG  = 45225
};

struct npc_harrison_jones_zaAI : public npc_escortAI
{
    npc_harrison_jones_zaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void WaypointReached(uint32 uiPointId) override
    {
        if (!m_pInstance)
            return;

        switch (uiPointId)
        {
            case 1:
                DoScriptText(SAY_AT_GONG, m_creature);

                m_pInstance->DoToggleGameObjectFlags(GO_STRANGE_GONG, GO_FLAG_NO_INTERACT, false);

                // Start bang gong for 2min
                DoCastSpellIfCan(m_creature, SPELL_BANGING_THE_GONG);
                SetEscortPaused(true);
                break;
            case 3:
                DoScriptText(SAY_OPEN_ENTRANCE, m_creature);
                break;
            case 4:
                m_pInstance->SetData(TYPE_EVENT_RUN, IN_PROGRESS);
                // TODO: Spawn group of Amani'shi Savage and make them run to entrance
                break;
        }
    }

    void Reset() override { }

    void StartEvent()
    {
        DoScriptText(SAY_START, m_creature);
        Start();
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);

        // Stop banging gong if still
        if (m_pInstance && m_pInstance->GetData(TYPE_EVENT_RUN) == SPECIAL && m_creature->HasAura(SPELL_BANGING_THE_GONG))
            m_creature->RemoveAurasDueToSpell(SPELL_BANGING_THE_GONG);
    }
};

bool GossipHello_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pInstance && pInstance->GetData(TYPE_EVENT_RUN) == NOT_STARTED)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
            pHarrisonAI->StartEvent();

        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

CreatureAI* GetAI_npc_harrison_jones_za(Creature* pCreature)
{
    return new npc_harrison_jones_zaAI(pCreature);
}

/*######
## go_strange_gong
######*/

// Unsure how this Gong must work. Here we always return false to allow Mangos always process further.
bool GOUse_go_strange_gong(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_EVENT_RUN) == SPECIAL)
    {
        if (Creature* pCreature = pInstance->GetSingleCreatureFromStorage(NPC_HARRISON))
        {
            if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
                pHarrisonAI->SetHoldState(false);
        }
        else
            script_error_log("Instance Zulaman: go_strange_gong failed");

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        return false;
    }

    pInstance->SetData(TYPE_EVENT_RUN, SPECIAL);
    return false;
}

void AddSC_zulaman()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_forest_frog";
    pNewScript->GetAI = &GetAI_npc_forest_frog;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harrison_jones_za";
    pNewScript->GetAI = &GetAI_npc_harrison_jones_za;
    pNewScript->pGossipHello =  &GossipHello_npc_harrison_jones_za;
    pNewScript->pGossipSelect = &GossipSelect_npc_harrison_jones_za;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_strange_gong";
    pNewScript->pGOUse = &GOUse_go_strange_gong;
    pNewScript->RegisterSelf();
}
