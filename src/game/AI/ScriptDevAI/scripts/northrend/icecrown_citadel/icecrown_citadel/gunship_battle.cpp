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
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    // gossip options
    GOSSIP_ITEM_ID_START_HORDE      = -3631006,
    GOSSIP_ITEM_ID_START_ALLIANCE   = -3631007,

    // gossip texts
    TEXT_ID_START_HORDE             = 15219,
    TEXT_ID_START_ALLIANCE          = 15101,

    // spells
    // SPELL_FRIENDLY_BOSS_DAMAGE_MOD  = 70339,         // handled in EAI
    SPELL_TELEPORT_PLAYERS_RESET_A  = 70446,
    SPELL_TELEPORT_PLAYERS_RESET_H  = 71284,
    SPELL_GUNSHIP_FALL_TELEPORT     = 67335,
    SPELL_LOCK_PLAYERS_TAP_CHEST    = 72347,            // targets creature 38569
    // SPELL_SKYBREAKER_DECK        = 70120,            // applied by creature 37519 on the Alliance ship; handled in creature_addon
    // SPELL_ORGRIMS_HAMMER_DECK    = 70121,            // applied by creature 37519 on the Horde ship; handled in creature_addon
    SPELL_HATE_TO_ZERO              = 63984,

    SPELL_MELEE_TARGETING_A         = 70219,            // cast by horde soldiers: 36957, 36960 to target hostile players
    SPELL_MELEE_TARGETING_H         = 70294,            // cast by alliance soldiers 36950 and 36961 to target hostile players

    SPELL_EXPLOSION_FAIL            = 72134,
    SPELL_EXPLOSION_VICTORY         = 72137,            // cast by creature 37547 on enemy ship

    SPELL_TELEPORT_ENEMY_SHIP       = 70104,            // cast by enemy combatants when teleporting to ship; soldiers are spawned on enemy ship and then teleport to player ship with this spell
    SPELL_BERSERK                   = 72525,

    SPELL_BATTLE_EXPERIENCE         = 71201,            // cast by enemy soldiers; related to 71188, 71193, 71195
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
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
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
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

/*######
## spell_incinerating_blast - 69402, 70175
######*/

struct spell_incinerating_blast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        if (effIdx == EFFECT_INDEX_1)
        {
            uint32 damage = spell->GetDamage();
            uint32 energy = caster->GetPower(caster->GetPowerType());

            // Note: this calculation has to be verified
            spell->SetDamage(damage + energy * energy * 8);
        }
        // remove all power
        else if (effIdx == EFFECT_INDEX_2)
            caster->SetPower(caster->GetPowerType(), 0);
    }
};

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

    RegisterSpellScript<spell_incinerating_blast>("spell_incinerating_blast");
}
