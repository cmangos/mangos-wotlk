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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/eastern_kingdoms/world_eastern_kingdoms.h"
#include "AI/ScriptDevAI/scripts/kalimdor/world_kalimdor.h"

struct BrewfestMountTransformation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == ALLIANCE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct BrewfestMountTransformationFactionSwap : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == HORDE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

std::vector<uint32> belbiTexts = { 22170, 22171, 22172, 22173, 22174, 22175 };
std::vector<uint32> blixTexts = { 23497, 23498, 23499, 23500, 23501, 23502 };
std::vector<uint32> itaTexts = { 22135, 22136, 22137 };
std::vector<uint32> maeveTexts = { 22132, 22133, 22134 };
std::vector<uint32> gordokTexts = { 22138, 22139, 22140 };
std::vector<uint32> drohnTexts = { 23513, 23514, 23515, 23516 };
std::vector<uint32> tchaliTexts = { 23517, 23518, 23519 };
std::vector<uint32> ipfelkoferTexts = { 23691, 23692, 23693 };
std::vector<uint32> tapperTexts = { 23698, 23700, 23701 };

struct npc_brewfest_barker : public ScriptedAI
{
    npc_brewfest_barker(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_bCanStartScript;
    uint32 m_uiScriptCooldownTimer;
    uint32 m_uiEmoteTimer;

    void Reset() override
    {
        m_uiScriptCooldownTimer = 0;
        m_uiEmoteTimer          = 0;
        m_bCanStartScript       = true;
    }

    void StartScript(Player* player)
    {
        m_bCanStartScript = false;

        switch (m_creature->GetEntry())
        {
            // ticket redeemers
            case NPC_BELBI_QUIKSWITCH:
            case NPC_BLIX_FIXWIDGET:
                m_creature->HandleEmote(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? EMOTE_ONESHOT_EXCLAMATION : EMOTE_ONESHOT_TALK_NOSHEATHE);
                m_uiScriptCooldownTimer = 10000;
                m_uiEmoteTimer = 3000;
                break;
            // organizers
            case NPC_IPFELKOFER_IRONKEG:
            case NPC_TAPPER_SWINDLEKEG:
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                m_uiScriptCooldownTimer = 30000;
                break;
            // barkers
            default:
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK_NOSHEATHE);
                m_uiScriptCooldownTimer = urand(30000, 120000);
                break;
        }

        switch (m_creature->GetEntry())
        {
            // ticket redeemers
            case NPC_BELBI_QUIKSWITCH: DoBroadcastText(belbiTexts[urand(0, belbiTexts.size() - 1)], m_creature, player); break;
            case NPC_BLIX_FIXWIDGET: DoBroadcastText(blixTexts[urand(0, blixTexts.size() - 1)], m_creature, player); break;
            // organizers
            case NPC_IPFELKOFER_IRONKEG: DoBroadcastText(ipfelkoferTexts[urand(0, ipfelkoferTexts.size() - 1)], m_creature, player); break;
            case NPC_TAPPER_SWINDLEKEG: DoBroadcastText(tapperTexts[urand(0, tapperTexts.size() - 1)], m_creature, player); break;
            // barkers
            case NPC_ITA_THUNDERBREW: DoBroadcastText(itaTexts[urand(0, itaTexts.size() - 1)], m_creature, player); break;
            case NPC_MAEVE_BARLEYBREW: DoBroadcastText(maeveTexts[urand(0, maeveTexts.size() - 1)], m_creature, player); break;
            case NPC_GORDOK_BREW_BARKER: DoBroadcastText(gordokTexts[urand(0, gordokTexts.size() - 1)], m_creature, player); break;
            case NPC_DROHNS_DISTILLERY_BARKER: DoBroadcastText(drohnTexts[urand(0, drohnTexts.size() - 1)], m_creature, player); break;
            case NPC_TCHALIS_VOODOO_BREWERY_BARKER: DoBroadcastText(tchaliTexts[urand(0, tchaliTexts.size() - 1)], m_creature, player); break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiScriptCooldownTimer)
        {
            if (m_uiScriptCooldownTimer < diff)
            {
                m_uiScriptCooldownTimer = 0;
                m_bCanStartScript = true;
            }
            else
                m_uiScriptCooldownTimer -= diff;
        }

        if (m_uiEmoteTimer)
        {
            if (m_uiEmoteTimer < diff)
            {
                m_creature->HandleEmote(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? EMOTE_ONESHOT_SHY : EMOTE_ONESHOT_YES);
                m_uiEmoteTimer = 0;
            }
            else
                m_uiEmoteTimer -= diff;
        }
    }
};

enum
{
    AT_ITA_THUNDERBREW                  = 4712,
    AT_MAEVE_BARLEYBREW                 = 4715,
    AT_GORDOK_BREW_BARKER_ALLY          = 4716,
    AT_BELBI_QUIKSWITCH                 = 4718,
    AT_GORDOK_BREW_BARKER_HORDE         = 4797,
    AT_DROHNS_DISTILLERY_BARKER         = 4798,
    AT_TCHALIS_VOODOO_BREWERY_BARKER    = 4799,
    AT_BLIX_FIXWIDGET                   = 4800,
    AT_IPFELKOFER_IRONKEG               = 4820,
    AT_TAPPER_SWINDLEKEG                = 4829,
};

bool AreaTrigger_at_brewfest_barker(Player* player, AreaTriggerEntry const* pAt)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    uint32 barkerEntry = 0;
    switch (pAt->id)
    {
        // ticket redeemers
        case AT_BELBI_QUIKSWITCH: barkerEntry = NPC_BELBI_QUIKSWITCH; break;
        case AT_BLIX_FIXWIDGET: barkerEntry = NPC_BLIX_FIXWIDGET; break;
        // organizers
        case AT_IPFELKOFER_IRONKEG: barkerEntry = NPC_IPFELKOFER_IRONKEG; break;
        case AT_TAPPER_SWINDLEKEG: barkerEntry = NPC_TAPPER_SWINDLEKEG; break;
        // barkers (should not respond during Dark Iron Attack event?)
        case AT_ITA_THUNDERBREW: barkerEntry = NPC_ITA_THUNDERBREW; break;
        case AT_MAEVE_BARLEYBREW: barkerEntry = NPC_MAEVE_BARLEYBREW; break;
        case AT_GORDOK_BREW_BARKER_ALLY:
        case AT_GORDOK_BREW_BARKER_HORDE:
            barkerEntry = NPC_GORDOK_BREW_BARKER;
            break;
        case AT_DROHNS_DISTILLERY_BARKER: barkerEntry = NPC_DROHNS_DISTILLERY_BARKER; break;
        case AT_TCHALIS_VOODOO_BREWERY_BARKER: barkerEntry = NPC_TCHALIS_VOODOO_BREWERY_BARKER; break;
    }

    if (Creature* barker = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(barkerEntry))
        if (npc_brewfest_barker* barkerAI = dynamic_cast<npc_brewfest_barker*>(barker->AI()))
            if (barkerAI->m_bCanStartScript)
                barkerAI->StartScript(player);

    return true;
}

void AddSC_brewfest()
{
    RegisterSpellScript<BrewfestMountTransformation>("spell_brewfest_mount_transformation");
    RegisterSpellScript<BrewfestMountTransformationFactionSwap>("spell_brewfest_mount_transformation_faction_swap");

    Script* pNewScript = new Script;
    pNewScript->Name = "npc_brewfest_barker";
    pNewScript->GetAI = &GetNewAIInstance<npc_brewfest_barker>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_brewfest_barker";
    pNewScript->pAreaTrigger = &AreaTrigger_at_brewfest_barker;
    pNewScript->RegisterSelf();
}