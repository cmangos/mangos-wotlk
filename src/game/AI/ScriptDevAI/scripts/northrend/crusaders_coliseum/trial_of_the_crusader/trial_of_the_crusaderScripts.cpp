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
SDName: trial_of_the_crusader
SD%Complete: 0
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"

enum
{
    GOSSIP_TEXT_BEAST_INIT              = 14664,
    GOSSIP_TEXT_BEAST_START             = 14665,
    GOSSIP_TEXT_BEAST_WIPE_INIT         = 14667,
    GOSSIP_TEXT_BEAST_WIPE_START        = 14668,

    GOSSIP_TEXT_JARAXXUS_INIT           = 14678,
    GOSSIP_TEXT_JARAXXUS_START          = 14680,
    GOSSIP_TEXT_JARAXXUS_WIPE_INIT      = 14679,
    GOSSIP_TEXT_JARAXXUS_WIPE_START     = 14682,

    GOSSIP_TEXT_PVP_INIT                = 14813,
    GOSSIP_TEXT_PVP_START               = 14814,
    GOSSIP_TEXT_PVP_WIPE_INIT           = 14815,
    GOSSIP_TEXT_PVP_WIPE_START          = 14816,

    GOSSIP_TEXT_TWINS_INIT              = 14819,
    GOSSIP_TEXT_TWINS_START             = 14821,
    GOSSIP_TEXT_TWINS_WIPE_INIT         = 14820,
    GOSSIP_TEXT_TWINS_WIPE_START        = 14822,

    GOSSIP_TEXT_ANUB_INIT               = 14828,
    GOSSIP_TEXT_ANUB_START              = 14829,

    GOSSIP_ITEM_BEAST_INIT              = -3649000,
    GOSSIP_ITEM_BEAST_START             = -3649001,
    GOSSIP_ITEM_BEAST_WIPE_INIT         = -3649002,
    GOSSIP_ITEM_BEAST_WIPE_START        = -3649014,

    GOSSIP_ITEM_JARAXXUS_INIT           = -3649003,
    GOSSIP_ITEM_JARAXXUS_START          = -3649011,
    GOSSIP_ITEM_JARAXXUS_WIPE_INIT      = -3649004,
    GOSSIP_ITEM_JARAXXUS_WIPE_START     = -3649015,

    GOSSIP_ITEM_PVP_INIT                = -3649005,
    GOSSIP_ITEM_PVP_START               = -3649006,
    GOSSIP_ITEM_PVP_WIPE_INIT           = -3649012,
    GOSSIP_ITEM_PVP_WIPE_START          = -3649013,

    GOSSIP_ITEM_TWINS_INIT              = -3649007,
    GOSSIP_ITEM_TWINS_START             = -3649008,
    GOSSIP_ITEM_TWINS_WIPE_INIT         = -3649016,
    GOSSIP_ITEM_TWINS_WIPE_START        = -3649017,

    GOSSIP_ITEM_ANUB_INIT               = -3649009,
    GOSSIP_ITEM_ANUB_START              = -3649010,
};

/*######
## npc_barrett_ramsey
######*/

struct RamseyInfo
{
    uint32 uiEntry;
    uint32 uiTextEntry;
    int32  iGossipItem;
    uint32 uiWipeTextEntry;
    int32  iWipeGossipItem;
    uint32 uiOptionId;                                      // If . > 0 SetInstData(. , SPECIAL), else open new DiagMenu
};

static const RamseyInfo aRamseyInfo[] =
{
    {NPC_RAMSEY_1, GOSSIP_TEXT_BEAST_INIT,  GOSSIP_ITEM_BEAST_INIT,  GOSSIP_TEXT_BEAST_WIPE_INIT,  GOSSIP_ITEM_BEAST_WIPE_INIT,  0},
    {NPC_RAMSEY_1, GOSSIP_TEXT_BEAST_START, GOSSIP_ITEM_BEAST_START, GOSSIP_TEXT_BEAST_WIPE_START, GOSSIP_ITEM_BEAST_WIPE_START, TYPE_NORTHREND_BEASTS},

    {NPC_RAMSEY_2, GOSSIP_TEXT_JARAXXUS_INIT,  GOSSIP_ITEM_JARAXXUS_INIT,  GOSSIP_TEXT_JARAXXUS_WIPE_INIT,  GOSSIP_ITEM_JARAXXUS_WIPE_INIT,  0},
    {NPC_RAMSEY_2, GOSSIP_TEXT_JARAXXUS_START, GOSSIP_ITEM_JARAXXUS_START, GOSSIP_TEXT_JARAXXUS_WIPE_START, GOSSIP_ITEM_JARAXXUS_WIPE_START, TYPE_JARAXXUS},

    {NPC_RAMSEY_3, GOSSIP_TEXT_PVP_INIT,  GOSSIP_ITEM_PVP_INIT,  GOSSIP_TEXT_PVP_WIPE_INIT,  GOSSIP_ITEM_PVP_WIPE_INIT,  0},
    {NPC_RAMSEY_3, GOSSIP_TEXT_PVP_START, GOSSIP_ITEM_PVP_START, GOSSIP_TEXT_PVP_WIPE_START, GOSSIP_ITEM_PVP_WIPE_START, TYPE_FACTION_CHAMPIONS},

    {NPC_RAMSEY_4, GOSSIP_TEXT_TWINS_INIT,  GOSSIP_ITEM_TWINS_INIT,  GOSSIP_TEXT_TWINS_WIPE_INIT,  GOSSIP_ITEM_TWINS_WIPE_INIT,  0},
    {NPC_RAMSEY_4, GOSSIP_TEXT_TWINS_START, GOSSIP_ITEM_TWINS_START, GOSSIP_TEXT_TWINS_WIPE_START, GOSSIP_ITEM_TWINS_WIPE_START, TYPE_TWIN_VALKYR},

    {NPC_RAMSEY_5, GOSSIP_TEXT_ANUB_INIT,  GOSSIP_ITEM_ANUB_INIT,  0,  0,  0},
    {NPC_RAMSEY_5, GOSSIP_TEXT_ANUB_START, GOSSIP_ITEM_ANUB_START, 0, 0, TYPE_ANUBARAK},
};

struct npc_barrett_ramseyAI : public ScriptedAI
{
    npc_barrett_ramseyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ScriptedInstance* m_pInstance;

    void Reset() override {}

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == 1)
            m_creature->ForcedDespawn();
    }
};

bool GossipHello_npc_barrett_ramsey(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    uint8 uiPos = 0;
    uint32 uiType = 0;

    for (uint8 i = 0; i < countof(aRamseyInfo); ++i)
    {
        if (pCreature->GetEntry() == aRamseyInfo[i].uiEntry)
        {
            if (!aRamseyInfo[i].uiOptionId)
                uiPos = i;
            else
            {
                uiType = aRamseyInfo[i].uiOptionId;
                break;
            }
        }
    }

    if (!uiType || !pInstance)
        return true;

    if (pInstance->GetData(uiType) == FAIL)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, aRamseyInfo[uiPos].iWipeGossipItem, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(aRamseyInfo[uiPos].uiWipeTextEntry, pCreature->GetObjectGuid());
    }
    else
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, aRamseyInfo[uiPos].iGossipItem, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(aRamseyInfo[uiPos].uiTextEntry, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_barrett_ramsey(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!pInstance)
        return true;

    if (uiAction > GOSSIP_ACTION_INFO_DEF)
    {
        // Begin Event
        uint32 uiType = uiAction - GOSSIP_ACTION_INFO_DEF;
        if (pInstance->GetData(uiType) == FAIL || pInstance->GetData(uiType) == NOT_STARTED)
            pInstance->SetData(uiAction - GOSSIP_ACTION_INFO_DEF, SPECIAL);

        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->GetMotionMaster()->MovePoint(1, aRamsayPositions[1][0], aRamsayPositions[1][1], aRamsayPositions[1][2]);

        return true;
    }

    for (const auto& i : aRamseyInfo)
    {
        if (pCreature->GetEntry() == i.uiEntry && i.uiOptionId)
        {
            if (pInstance->GetData(i.uiOptionId) == FAIL)
            {
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, i.iWipeGossipItem, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + i.uiOptionId);
                pPlayer->SEND_GOSSIP_MENU(i.uiWipeTextEntry, pCreature->GetObjectGuid());
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, i.iGossipItem, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + i.uiOptionId);
                pPlayer->SEND_GOSSIP_MENU(i.uiTextEntry, pCreature->GetObjectGuid());
            }

            return true;
        }
    }

    return true;
}

UnitAI* GetAI_npc_barrett_ramsey(Creature* pCreature)
{
    return new npc_barrett_ramseyAI(pCreature);
}

void AddSC_trial_of_the_crusader()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_barrett_ramsey";
    pNewScript->GetAI = &GetAI_npc_barrett_ramsey;
    pNewScript->pGossipHello = &GossipHello_npc_barrett_ramsey;
    pNewScript->pGossipSelect = &GossipSelect_npc_barrett_ramsey;
    pNewScript->RegisterSelf();
}
