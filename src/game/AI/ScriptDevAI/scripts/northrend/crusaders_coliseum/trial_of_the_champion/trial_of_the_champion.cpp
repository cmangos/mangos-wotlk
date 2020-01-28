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
SDName: trial_of_the_champion
SD%Complete: 20
SDComment: Gossip
SDCategory: Crusader Coliseum, Trial of the Champion
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_champion.h"

/*######
## npc_toc_herald
######*/

enum
{
    GOSSIP_ITEM_READY                   = -3650000,
    GOSSIP_ITEM_READY_SKIP_INTRO        = -3650001,
    GOSSIP_ITEM_READY_NEXT_CHALLENGE    = -3650002,

    TEXT_ID_READY_FIRST_CHALLENGE       = 14688,
    TEXT_ID_READY_NEXT_CHALLENGE        = 14737,
    TEXT_ID_READY_FINAL_CHALLENGE       = 14738,
};

bool GossipHello_npc_toc_herald(Player* pPlayer, Creature* pCreature)
{
    instance_trial_of_the_champion* pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_GRAND_CHAMPIONS) == NOT_STARTED)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY_SKIP_INTRO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY_FIRST_CHALLENGE, pCreature->GetObjectGuid());
    }
    else if (pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE && pInstance->GetData(TYPE_ARGENT_CHAMPION) == NOT_STARTED)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY_NEXT_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY_NEXT_CHALLENGE, pCreature->GetObjectGuid());
    }
    else if (pInstance->GetData(TYPE_ARGENT_CHAMPION) == DONE && pInstance->GetData(TYPE_BLACK_KNIGHT) == NOT_STARTED)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY_FINAL_CHALLENGE, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_toc_herald(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 uiAction)
{
    instance_trial_of_the_champion* pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
    if (!pInstance)
        return true;

    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pInstance->DoPrepareChampions(false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pInstance->DoPrepareChampions(true);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pInstance->DoPrepareArgentChallenge();
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pInstance->DoPrepareBlackKnight();
            break;
    }

    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

void AddSC_trial_of_the_champion()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_toc_herald";
    pNewScript->pGossipHello = &GossipHello_npc_toc_herald;
    pNewScript->pGossipSelect = &GossipSelect_npc_toc_herald;
    pNewScript->RegisterSelf();
}
