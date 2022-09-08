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
SDName: Npc_Professions
SD%Complete: 90
SDComment: Provides learn/unlearn/relearn options for profession specializations. Not supported: Unlearn engineering, re-learn engineering
SDCategory: NPCs
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

/*
A few notes for future developement:
- A full implementation of gossip for GO's is required. They must have the same scripting capabilities as creatures. Basically,
there is no difference here (except that default text is chosen with `gameobject_template`.`data3` (for GO type2, different dataN for a few others)
- It's possible blacksmithing still require some tweaks and adjustments due to the way we _have_ to use reputation.
*/

/*
-- UPDATE `gameobject_template` SET `ScriptName` = 'go_soothsaying_for_dummies' WHERE `entry` = 177226;
*/

/*###
# generic defines
###*/

#define GOSSIP_SENDER_LEARN         50
#define GOSSIP_SENDER_UNLEARN       51
#define GOSSIP_SENDER_CHECK         52

/*###
# gossip item and box texts
###*/

#define GOSSIP_UNLEARN_DRAGON		"I wish to unlearn my Dragonscale Leatherworking specialization!"
#define BOX_UNLEARN_DRAGON			"Do you really want to unlearn your Dragonscale Leatherworking specialization and lose all associated recipes?\n"
#define GOSSIP_LEARN_DRAGON			"Please teach me how to become a Dragonscale leatherworker."

#define GOSSIP_UNLEARN_ELEMENTAL	"I wish to unlearn my Elemental Leatherworking specialization!"
#define BOX_UNLEARN_ELEMENTAL		"Do you really want to unlearn your Elemental Leatherworking specialization and lose all associated recipes?\n"
#define GOSSIP_LEARN_ELEMENTAL		"Please teach me how to become an Elemental leatherworker."

#define GOSSIP_UNLEARN_TRIBAL		"I wish to unlearn my Tribal Leatherworking specialization!"
#define BOX_UNLEARN_TRIBAL			"Do you really want to unlearn your Tribal Leatherworking specialization and lose all associated recipes?\n"
#define GOSSIP_LEARN_TRIBAL			"Please teach me how to become a Tribal leatherworker."

enum SpecializationSpells
{
    /* Leatherworking */
    S_DRAGON                = 10656,
    S_ELEMENTAL             = 10658,
    S_TRIBAL                = 10660,
    S_LEARN_DRAGON          = 10657,
    S_LEARN_ELEMENTAL       = 10659,
    S_LEARN_TRIBAL          = 10661,
    S_UNLEARN_DRAGON        = 36434,
    S_UNLEARN_ELEMENTAL     = 36328,
    S_UNLEARN_TRIBAL        = 36433,

    /* Tailoring */
    S_SPELLFIRE             = 26797,
    S_MOONCLOTH             = 26798,
    S_SHADOWEAVE            = 26801,
    S_LEARN_SPELLFIRE       = 26796,
    S_LEARN_MOONCLOTH       = 26799,
    S_LEARN_SHADOWEAVE      = 26800,
    S_UNLEARN_SPELLFIRE     = 41299,
    S_UNLEARN_MOONCLOTH     = 41558,
    S_UNLEARN_SHADOWEAVE    = 41559,
};

enum SpecializationQuests
{
    /* Leatherworking */
    Q_DRAGON_ALLY           = 5141,
    Q_DRAGON_HORDE          = 5145,
    Q_ELEMENTAL_ALLY        = 5144,
    Q_ELEMENTAL_HORDE       = 5146,
    Q_TRIBAL_ALLY           = 5143,
    Q_TRIBAL_HORDE          = 5148,
};

enum SpecializationTrainers
{
    /* Leatherworking */
    N_TRAINER_DRAGON1       = 7866,		// Peter Galen
    N_TRAINER_DRAGON2       = 7867,		// Thorkaf Dragoneye
    N_TRAINER_ELEMENTAL1    = 7868,		// Sarah Tanner
    N_TRAINER_ELEMENTAL2    = 7869,		// Brumn Winterhoof
    N_TRAINER_TRIBAL1       = 7870,		// Caryssia Moonhunter
    N_TRAINER_TRIBAL2       = 7871,		// Se'Jib
};

/*###
# formulas to calculate unlearning cost
###*/

int32 GetUnlearnCostMedium(Player* pPlayer)                 // blacksmith, leatherwork
{
    uint32 level = pPlayer->GetLevel();

    if (level < 51)
        return 250000;
    if (level < 66)
        return 500000;
    return 1000000;
}


/*###
# unlearning related profession spells
###*/

bool EquippedOk(Player* pPlayer, uint32 spellId)
{
    SpellEntry const* spell = GetSpellStore()->LookupEntry<SpellEntry>(spellId);

    if (!spell)
        return false;

    for (unsigned int reqSpell : spell->EffectTriggerSpell)
    {
        if (!reqSpell)
            continue;

        for (int j = EQUIPMENT_SLOT_START; j < EQUIPMENT_SLOT_END; ++j)
        {
            Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, j);
            if (pItem)
                if (pItem->GetProto()->RequiredSpell == reqSpell)
                {
                    // pPlayer has item equipped that require specialty. Not allow to unlearn, player has to unequip first
                    debug_log("SD2: player attempt to unlearn spell %u, but item %u is equipped.", reqSpell, pItem->GetProto()->ItemId);
                    return false;
                }
        }
    }
    return true;
}

void ProfessionUnlearnSpells(Player* pPlayer, uint32 type)
{
    switch (type)
    {
        case S_UNLEARN_DRAGON:
            pPlayer->removeSpell(36076);                    // Dragonstrike Leggings
            pPlayer->removeSpell(36079);                    // Golden Dragonstrike Breastplate
            pPlayer->removeSpell(35576);                    // Ebon Netherscale Belt
            pPlayer->removeSpell(35577);                    // Ebon Netherscale Bracers
            pPlayer->removeSpell(35575);                    // Ebon Netherscale Breastplate
            pPlayer->removeSpell(35582);                    // Netherstrike Belt
            pPlayer->removeSpell(35584);                    // Netherstrike Bracers
            pPlayer->removeSpell(35580);                    // Netherstrike Breastplate
            break;
        case S_UNLEARN_ELEMENTAL:
            pPlayer->removeSpell(36074);                    // Blackstorm Leggings
            pPlayer->removeSpell(36077);                    // Primalstorm Breastplate
            pPlayer->removeSpell(35590);                    // Primalstrike Belt
            pPlayer->removeSpell(35591);                    // Primalstrike Bracers
            pPlayer->removeSpell(35589);                    // Primalstrike Vest
            break;
        case S_UNLEARN_TRIBAL:
            pPlayer->removeSpell(35585);                    // Windhawk Hauberk
            pPlayer->removeSpell(35587);                    // Windhawk Belt
            pPlayer->removeSpell(35588);                    // Windhawk Bracers
            pPlayer->removeSpell(36075);                    // Wildfeather Leggings
            pPlayer->removeSpell(36078);                    // Living Crystal Breastplate
            break;
        case S_UNLEARN_SPELLFIRE:
            pPlayer->removeSpell(26752);                    // Spellfire Belt
            pPlayer->removeSpell(26753);                    // Spellfire Gloves
            pPlayer->removeSpell(26754);                    // Spellfire Robe
            break;
        case S_UNLEARN_MOONCLOTH:
            pPlayer->removeSpell(26760);                    // Primal Mooncloth Belt
            pPlayer->removeSpell(26761);                    // Primal Mooncloth Shoulders
            pPlayer->removeSpell(26762);                    // Primal Mooncloth Robe
            break;
        case S_UNLEARN_SHADOWEAVE:
            pPlayer->removeSpell(26756);                    // Frozen Shadoweave Shoulders
            pPlayer->removeSpell(26757);                    // Frozen Shadoweave Boots
            pPlayer->removeSpell(26758);                    // Frozen Shadoweave Robe
            break;
    }
}

/*###
# start menues leatherworking
###*/

bool IsEligibleSpecializeLW(Player* pPlayer)
{
    // Allow re-learn specialization as long as player is not specialized currently and has completed any specialization quest
    if ((!pPlayer->HasSpell(S_DRAGON) && !pPlayer->HasSpell(S_ELEMENTAL) && !pPlayer->HasSpell(S_TRIBAL))
        && (pPlayer->GetQuestRewardStatus(Q_DRAGON_ALLY)
            || pPlayer->GetQuestRewardStatus(Q_DRAGON_HORDE)
            || pPlayer->GetQuestRewardStatus(Q_ELEMENTAL_ALLY)
            || pPlayer->GetQuestRewardStatus(Q_ELEMENTAL_HORDE)
            || pPlayer->GetQuestRewardStatus(Q_TRIBAL_ALLY)
            || pPlayer->GetQuestRewardStatus(Q_TRIBAL_HORDE)))
        return true;
    return false;
}

bool GossipHello_npc_prof_leather(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    uint32 eCreature = pCreature->GetEntry();

    if (pPlayer->HasSkill(SKILL_LEATHERWORKING) && pPlayer->GetSkillValueBase(SKILL_LEATHERWORKING) >= 225 && pPlayer->GetLevel() > 39)
    {
        switch (eCreature)
        {
            case N_TRAINER_DRAGON1:
            case N_TRAINER_DRAGON2:
                if (pPlayer->HasSpell(S_DRAGON))
                {
                    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_UNLEARN_DRAGON, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 1, BOX_UNLEARN_DRAGON, GetUnlearnCostMedium(pPlayer), false);
                    if (pCreature->isTrainer())
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                }
                else if (IsEligibleSpecializeLW(pPlayer))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_DRAGON, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 2);
                break;
            case N_TRAINER_ELEMENTAL1:
            case N_TRAINER_ELEMENTAL2:
                if (pPlayer->HasSpell(S_ELEMENTAL))
                {
                    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_UNLEARN_ELEMENTAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 3, BOX_UNLEARN_ELEMENTAL, GetUnlearnCostMedium(pPlayer), false);
                    if (pCreature->isTrainer())
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                }
                else if (IsEligibleSpecializeLW(pPlayer))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_ELEMENTAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 4);
                break;
            case N_TRAINER_TRIBAL1:
            case N_TRAINER_TRIBAL2:
                if (pPlayer->HasSpell(S_TRIBAL))
                {
                    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_UNLEARN_TRIBAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 5, BOX_UNLEARN_TRIBAL, GetUnlearnCostMedium(pPlayer), false);
                    if (pCreature->isTrainer())
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                }
                else if (IsEligibleSpecializeLW(pPlayer))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_TRIBAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 6);
                break;
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

void SendActionMenu_npc_prof_leather(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
        // Unlearn Dragonscale Leatherworking
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (EquippedOk(pPlayer, S_UNLEARN_DRAGON))
            {
                if (pPlayer->GetMoney() >= uint32(GetUnlearnCostMedium(pPlayer)))
                {
                    pPlayer->CastSpell(pPlayer, S_UNLEARN_DRAGON, TRIGGERED_OLD_TRIGGERED);
                    ProfessionUnlearnSpells(pPlayer, S_UNLEARN_DRAGON);
                    pPlayer->ModifyMoney(-GetUnlearnCostMedium(pPlayer));
                }
                else
                    pPlayer->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, pCreature, 0, 0);
            }
            else
                pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, nullptr, nullptr);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:	// Learn Dragonscale Leatherworking
            pPlayer->CastSpell(pPlayer, S_LEARN_DRAGON, TRIGGERED_OLD_TRIGGERED);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:	// Unlearn Elemental Leatherworking
            if (EquippedOk(pPlayer, S_UNLEARN_ELEMENTAL))
            {
                if (pPlayer->GetMoney() >= uint32(GetUnlearnCostMedium(pPlayer)))
                {
                    pPlayer->CastSpell(pPlayer, S_UNLEARN_ELEMENTAL, TRIGGERED_OLD_TRIGGERED);
                    ProfessionUnlearnSpells(pPlayer, S_UNLEARN_ELEMENTAL);
                    pPlayer->ModifyMoney(-GetUnlearnCostMedium(pPlayer));
                }
                else
                    pPlayer->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, pCreature, 0, 0);
            }
            else
                pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, nullptr, nullptr);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:	// Learn Elemental Leatherworking
            pPlayer->CastSpell(pPlayer, S_LEARN_ELEMENTAL, TRIGGERED_OLD_TRIGGERED);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:	// Unlearn Tribal Leatherworking
            if (EquippedOk(pPlayer, S_UNLEARN_TRIBAL))
            {
                if (pPlayer->GetMoney() >= uint32(GetUnlearnCostMedium(pPlayer)))
                {
                    pPlayer->CastSpell(pPlayer, S_UNLEARN_TRIBAL, TRIGGERED_OLD_TRIGGERED);
                    ProfessionUnlearnSpells(pPlayer, S_UNLEARN_TRIBAL);
                    pPlayer->ModifyMoney(-GetUnlearnCostMedium(pPlayer));
                }
                else
                    pPlayer->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, pCreature, 0, 0);
            }
            else
                pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, nullptr, nullptr);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:	// Learn Tribal Leatherworking
            pPlayer->CastSpell(pPlayer, S_LEARN_TRIBAL, TRIGGERED_OLD_TRIGGERED);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
}

bool GossipSelect_npc_prof_leather(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiSender)
    {
        case GOSSIP_SENDER_MAIN:
        case GOSSIP_SENDER_CHECK:
            SendActionMenu_npc_prof_leather(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

/*###
# start menues for GO (engineering and leatherworking)
###*/

/*bool GOUse_go_soothsaying_for_dummies(Player* pPlayer, GameObject* pGo)
{
pPlayer->GetPlayerMenu()->GetGossipMenu()->AddMenuItem(0,GOSSIP_LEARN_DRAGON, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF, "", 0);

pPlayer->SEND_GOSSIP_MENU(5584, pGo->GetObjectGuid());

return true;
}*/

/*###
#
###*/

void AddSC_npc_professions()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_prof_leather";
    pNewScript->pGossipHello = &GossipHello_npc_prof_leather;
    pNewScript->pGossipSelect = &GossipSelect_npc_prof_leather;
    pNewScript->RegisterSelf();

    /*pNewScript = new Script;
    pNewScript->Name = "go_soothsaying_for_dummies";
    pNewScript->pGOUse =  &GOUse_go_soothsaying_for_dummies;
    // pNewScript->pGossipSelect = &GossipSelect_go_soothsaying_for_dummies;
    pNewScript->RegisterSelf();*/
}
