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
SDName: gunship_battle
SD%Complete: 20%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

enum
{
    // gossip options
    GOSSIP_ITEM_ID_START_HORDE      = -3631006,
    GOSSIP_ITEM_ID_START_ALLIANCE   = -3631007,

    // gossip texts
    TEXT_ID_START_HORDE             = 15219,
    TEXT_ID_START_ALLIANCE          = 15101,

    SPELL_ACHIEVEMENT_CHECK         = 72959,
};

bool GossipHello_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_HORDE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_HORDE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, IN_PROGRESS);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipHello_npc_muradin_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_ALLIANCE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_ALLIANCE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_muradin_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, IN_PROGRESS);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

void AddSC_gunship_battle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_saurfang_gunship";
    pNewScript->pGossipHello = &GossipHello_npc_saurfang_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_saurfang_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_muradin_gunship";
    pNewScript->pGossipHello = &GossipHello_npc_muradin_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_muradin_gunship;
    pNewScript->RegisterSelf();
}
