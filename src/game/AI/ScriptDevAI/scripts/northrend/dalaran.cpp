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
SDName: Dalaran
SD%Complete: 100
SDComment:
SDCategory: Dalaran
EndScriptData */

/* ContentData
npc_dalaran_guardian_mage
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI//scripts/northrend/world_northrend.h"

enum
{
    SPELL_TRESPASSER_H          = 54029,
    SPELL_TRESPASSER_A          = 54028,

    // Exception auras - used for quests 20439 and 24451
    SPELL_COVENANT_DISGUISE_1   = 70971,
    SPELL_COVENANT_DISGUISE_2   = 70972,
    SPELL_SUNREAVER_DISGUISE_1  = 70973,
    SPELL_SUNREAVER_DISGUISE_2  = 70974,

    AREA_ID_SUNREAVER           = 4616,
    AREA_ID_SILVER_ENCLAVE      = 4740
};

struct npc_dalaran_guardian_mageAI : public ScriptedAI
{
    npc_dalaran_guardian_mageAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void MoveInLineOfSight(Unit* who) override
    {
        if (m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE_MELEE)
            return;

        if (m_creature->IsEnemy(who))
        {
            // exception for quests 20439 and 24451
            if (who->HasAura(SPELL_COVENANT_DISGUISE_1) || who->HasAura(SPELL_COVENANT_DISGUISE_2) ||
                    who->HasAura(SPELL_SUNREAVER_DISGUISE_1) || who->HasAura(SPELL_SUNREAVER_DISGUISE_2))
                return;

            if (m_creature->IsWithinDistInMap(who, m_creature->GetAttackDistance(who)) && m_creature->IsWithinLOSInMap(who))
            {
                if (Player* pPlayer = who->GetBeneficiaryPlayer())
                {
                    // it's mentioned that pet may also be teleported, if so, we need to tune script to apply to those in addition.

                    if (pPlayer->GetAreaId() == AREA_ID_SILVER_ENCLAVE)
                        DoCastSpellIfCan(pPlayer, SPELL_TRESPASSER_A);
                    else if (pPlayer->GetAreaId() == AREA_ID_SUNREAVER)
                        DoCastSpellIfCan(pPlayer, SPELL_TRESPASSER_H);
                }
            }
        }
    }

    void AttackedBy(Unit* /*pAttacker*/) override {}

    void Reset() override {}

    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

/*######
## spell_teleporting_dalaran - 59317
######*/

struct spell_teleporting_dalaran : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        Player* playerTarget = static_cast<Player*>(target);

        // return from top
        if (playerTarget->GetAreaId() == 4637)
            target->CastSpell(target, 59316, TRIGGERED_OLD_TRIGGERED);
        // teleport atop
        else
            target->CastSpell(target, 59314, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## at_underbelly_h
######*/

enum
{
    NPC_SILVER_COVENANT_AGENT = 36774,
    ITEM_SILVER_COVENANT_ORDERS = 49872,
    QUEST_THE_SILVER_COVENANTS_SCHEME = 24557,
};

static const Position m_silvercovenantagent = { 5816.17f, 760.211f, 640.561f, 1.75213f }; // guesed xyz - pls change when correct found

bool AreaTrigger_at_underbelly_h(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    // Player is deaed, a GM, no quest or hasn't got item: do nothing
    if (!player->IsAlive() || player->IsGameMaster() ||
        player->GetQuestStatus(QUEST_THE_SILVER_COVENANTS_SCHEME) == QUEST_STATUS_NONE ||
        !player->HasItemCount(ITEM_SILVER_COVENANT_ORDERS, 1))
        return false;

    if (world_map_northrend* northrend = dynamic_cast<world_map_northrend*>(player->GetInstanceData()))
        if (northrend->IsDalaranCooldownForPlayer(player->GetObjectGuid()))
            return false;

    CreatureList agents;
    GetCreatureListWithEntryInGrid(agents, player, NPC_SILVER_COVENANT_AGENT, 50.0f);
    bool found = false;
    for (Creature* agent : agents)
    {
        if (agent->GetSpawnerGuid() == player->GetObjectGuid())
        {
            found = true;
            break;
        }
    }
    if (!found) // Summon Sunreaver Agent
    {
        if (Creature* silvercovenantagent = player->SummonCreature(NPC_SILVER_COVENANT_AGENT, m_silvercovenantagent.x, m_silvercovenantagent.y, m_silvercovenantagent.z, m_silvercovenantagent.o, TEMPSPAWN_TIMED_OOC_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
        {
            if (world_map_northrend* northrend = dynamic_cast<world_map_northrend*>(player->GetInstanceData()))
                northrend->SetDalaranCooldownTime(player->GetObjectGuid());
            silvercovenantagent->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_B, player, silvercovenantagent);
            return true;
        }
    }
    return false;
}

/*######
## at_underbelly_a
######*/

enum
{
    NPC_SUNREAVER_AGENT = 36776,
    ITEM_SUNREAVER_ORDERS = 49536,
    QUEST_THE_SUNREAVER_PLAN = 14457,
};

static const Position m_sunreaveragent = { 5783.11f, 534.748f, 641.56f, 6.27889f };

bool AreaTrigger_at_underbelly_a(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    // Player is deaed, a GM, no quest or hasn't got item: do nothing
    if (!player->IsAlive() || player->IsGameMaster() ||
        player->GetQuestStatus(QUEST_THE_SUNREAVER_PLAN) == QUEST_STATUS_NONE ||
        !player->HasItemCount(ITEM_SUNREAVER_ORDERS, 1))
        return false;

    if (world_map_northrend* northrend = dynamic_cast<world_map_northrend*>(player->GetInstanceData()))
        if (northrend->IsDalaranCooldownForPlayer(player->GetObjectGuid()))
            return false;

    CreatureList agents;
    GetCreatureListWithEntryInGrid(agents, player, NPC_SUNREAVER_AGENT, 50.0f);
    bool found = false;
    for (Creature* agent : agents)
    {
        if (agent->GetSpawnerGuid() == player->GetObjectGuid())
        {
            found = true;
            break;
        }
    }
    if (!found) // Summon Sunreaver Agent
    {
        if (Creature* sunreaveragent = player->SummonCreature(NPC_SUNREAVER_AGENT, m_sunreaveragent.x, m_sunreaveragent.y, m_sunreaveragent.z, m_sunreaveragent.o, TEMPSPAWN_TIMED_OOC_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
        {
            if (world_map_northrend* northrend = dynamic_cast<world_map_northrend*>(player->GetInstanceData()))
                northrend->SetDalaranCooldownTime(player->GetObjectGuid());
            sunreaveragent->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_B, player, sunreaveragent);
            return true;
        }
    }
    return false;
}

void AddSC_dalaran()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_dalaran_guardian_mage";
    pNewScript->GetAI = &GetNewAIInstance<npc_dalaran_guardian_mageAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_underbelly_h";
    pNewScript->pAreaTrigger = &AreaTrigger_at_underbelly_h;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_underbelly_a";
    pNewScript->pAreaTrigger = &AreaTrigger_at_underbelly_a;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_teleporting_dalaran>("spell_teleporting_dalaran");
}
