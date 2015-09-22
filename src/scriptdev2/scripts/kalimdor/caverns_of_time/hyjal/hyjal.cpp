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
SDName: Hyjal
SD%Complete: 80
SDComment: gossip text id's unknown
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

/* ContentData
npc_jaina_proudmoore
npc_thrall
npc_tyrande_whisperwind
EndContentData */

#include "precompiled.h"
#include "hyjalAI.h"

enum
{
    GOSSIP_ITEM_JAINA_BEGIN         = -3534000,
    GOSSIP_ITEM_JAINA_ANATHERON     = -3534001,
    GOSSIP_ITEM_JAINA_SUCCCESS      = -3534002,

    GOSSIP_ITEM_THRALL_BEGIN        = -3534003,
    GOSSIP_ITEM_THRALL_AZGALOR      = -3534004,
    GOSSIP_ITEM_THRALL_SUCCESS      = -3534005,

    GOSSIP_ITEM_TYRANDE_AID         = -3534006,

    // Note: additional menu items include 9230 and 9398.
    GOSSIP_MENU_ID_DEFAULT          = 907,                  // this is wrong, but currently we don't know which are the right ids
};

CreatureAI* GetAI_npc_jaina_proudmoore(Creature* pCreature)
{
    hyjalAI* pTempAI = new hyjalAI(pCreature);

    pTempAI->m_aSpells[0].m_uiSpellId = SPELL_BLIZZARD;
    pTempAI->m_aSpells[0].m_uiCooldown = urand(15000, 35000);
    pTempAI->m_aSpells[0].m_pType = TARGETTYPE_RANDOM;

    pTempAI->m_aSpells[1].m_uiSpellId = SPELL_PYROBLAST;
    pTempAI->m_aSpells[1].m_uiCooldown = urand(2000, 9000);
    pTempAI->m_aSpells[1].m_pType = TARGETTYPE_RANDOM;

    pTempAI->m_aSpells[2].m_uiSpellId = SPELL_SUMMON_ELEMENTALS;
    pTempAI->m_aSpells[2].m_uiCooldown = urand(15000, 45000);
    pTempAI->m_aSpells[2].m_pType = TARGETTYPE_SELF;

    return pTempAI;
}

bool GossipHello_npc_jaina_proudmoore(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (hyjalAI* pJainaAI = dynamic_cast<hyjalAI*>(pCreature->AI()))
        {
            if (!pJainaAI->IsEventInProgress())
            {
                // Should not happen that jaina is here now, but for safe we check
                if (pInstance->GetData(TYPE_KAZROGAL) != DONE)
                {
                    if (pInstance->GetData(TYPE_WINTERCHILL) == NOT_STARTED || pInstance->GetData(TYPE_WINTERCHILL) == FAIL)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_JAINA_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    else if (pInstance->GetData(TYPE_WINTERCHILL) == DONE && (pInstance->GetData(TYPE_ANETHERON) == NOT_STARTED || pInstance->GetData(TYPE_ANETHERON) == FAIL))
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_JAINA_ANATHERON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    else if (pInstance->GetData(TYPE_ANETHERON) == DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_JAINA_SUCCCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

                    if (pPlayer->isGameMaster())
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "[GM] Toggle Debug Timers", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                }
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_DEFAULT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_jaina_proudmoore(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (hyjalAI* pJainaAI = dynamic_cast<hyjalAI*>(pCreature->AI()))
    {
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                pJainaAI->StartEvent();
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                pJainaAI->m_bIsFirstBossDead = true;
                pJainaAI->m_uiWaveCount = 9;
                pJainaAI->StartEvent();
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                pJainaAI->Retreat();
                break;
            case GOSSIP_ACTION_INFO_DEF:
                pJainaAI->m_bDebugMode = !pJainaAI->m_bDebugMode;
                debug_log("SD2: HyjalAI - Debug mode has been toggled %s", pJainaAI->m_bDebugMode ? "on" : "off");
                break;
        }
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

CreatureAI* GetAI_npc_thrall(Creature* pCreature)
{
    hyjalAI* pTempAI = new hyjalAI(pCreature);

    pTempAI->m_aSpells[0].m_uiSpellId = SPELL_CHAIN_LIGHTNING;
    pTempAI->m_aSpells[0].m_uiCooldown = urand(2000, 7000);
    pTempAI->m_aSpells[0].m_pType = TARGETTYPE_VICTIM;

    pTempAI->m_aSpells[1].m_uiSpellId = SPELL_FERAL_SPIRIT;
    pTempAI->m_aSpells[1].m_uiCooldown = urand(6000, 41000);
    pTempAI->m_aSpells[1].m_pType = TARGETTYPE_RANDOM;

    return pTempAI;
}

bool GossipHello_npc_thrall(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (hyjalAI* pThrallAI = dynamic_cast<hyjalAI*>(pCreature->AI()))
        {
            if (!pThrallAI->IsEventInProgress())
            {
                // Only let them start the Horde phases if Anetheron is dead.
                if (pInstance->GetData(TYPE_ANETHERON) == DONE && pInstance->GetData(TYPE_ARCHIMONDE) != DONE)
                {
                    if (pInstance->GetData(TYPE_KAZROGAL) == NOT_STARTED || pInstance->GetData(TYPE_KAZROGAL) == FAIL)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THRALL_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    else if (pInstance->GetData(TYPE_KAZROGAL) == DONE && (pInstance->GetData(TYPE_AZGALOR) == NOT_STARTED || pInstance->GetData(TYPE_AZGALOR) == FAIL))
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THRALL_AZGALOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    else if (pInstance->GetData(TYPE_AZGALOR) == DONE)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THRALL_SUCCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

                    if (pPlayer->isGameMaster())
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "[GM] Toggle Debug Timers", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                }
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_DEFAULT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_thrall(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (hyjalAI* pThrallAI = dynamic_cast<hyjalAI*>(pCreature->AI()))
    {
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                pThrallAI->StartEvent();
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                pThrallAI->m_bIsFirstBossDead = true;
                pThrallAI->m_uiWaveCount = 9;
                pThrallAI->StartEvent();
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                pThrallAI->Retreat();
                break;
            case GOSSIP_ACTION_INFO_DEF:
                pThrallAI->m_bDebugMode = !pThrallAI->m_bDebugMode;
                debug_log("SD2: HyjalAI - Debug mode has been toggled %s", pThrallAI->m_bDebugMode ? "on" : "off");
                break;
        }
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipHello_npc_tyrande_whisperwind(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        // Only let them get item if Azgalor is dead.
        if (pInstance->GetData(TYPE_AZGALOR) == DONE && !pPlayer->HasItemCount(ITEM_TEAR_OF_GODDESS, 1))
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TYRANDE_AID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_DEFAULT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_tyrande_whisperwind(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_TEAR_OF_GODDESS, 1))
            pPlayer->SendNewItem(pItem, 1, true, false);
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_hyjal()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_jaina_proudmoore";
    pNewScript->GetAI = &GetAI_npc_jaina_proudmoore;
    pNewScript->pGossipHello = &GossipHello_npc_jaina_proudmoore;
    pNewScript->pGossipSelect = &GossipSelect_npc_jaina_proudmoore;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thrall";
    pNewScript->GetAI = &GetAI_npc_thrall;
    pNewScript->pGossipHello = &GossipHello_npc_thrall;
    pNewScript->pGossipSelect = &GossipSelect_npc_thrall;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tyrande_whisperwind";
    pNewScript->pGossipHello = &GossipHello_npc_tyrande_whisperwind;
    pNewScript->pGossipSelect = &GossipSelect_npc_tyrande_whisperwind;
    pNewScript->RegisterSelf();
}
