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

struct npc_ticket_redeemer : public ScriptedAI
{
    npc_ticket_redeemer(Creature* creature) : ScriptedAI(creature) { Reset(); }

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
        m_uiScriptCooldownTimer = 30000;
        m_uiEmoteTimer = 3000;
        m_creature->HandleEmote(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? EMOTE_ONESHOT_EXCLAMATION : EMOTE_ONESHOT_TALK_NOSHEATHE);
        DoBroadcastText(m_creature->GetEntry() == NPC_BELBI_QUIKSWITCH ? belbiTexts[urand(0, belbiTexts.size() - 1)] : blixTexts[urand(0, belbiTexts.size() - 1)], m_creature, player);
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

bool AreaTrigger_at_ticket_redeemer(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    if (Creature* redeemer = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(player->GetMapId() == 0 ? NPC_BELBI_QUIKSWITCH : NPC_BLIX_FIXWIDGET))
        if (npc_ticket_redeemer* ticketRedeemerAI = dynamic_cast<npc_ticket_redeemer*>(redeemer->AI()))
            if (ticketRedeemerAI->m_bCanStartScript)
                ticketRedeemerAI->StartScript(player);

    return true;
}

void AddSC_brewfest()
{
    RegisterSpellScript<BrewfestMountTransformation>("spell_brewfest_mount_transformation");
    RegisterSpellScript<BrewfestMountTransformationFactionSwap>("spell_brewfest_mount_transformation_faction_swap");

    Script* pNewScript = new Script;
    pNewScript->Name = "npc_ticket_redeemer";
    pNewScript->GetAI = &GetNewAIInstance<npc_ticket_redeemer>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_ticket_redeemer";
    pNewScript->pAreaTrigger = &AreaTrigger_at_ticket_redeemer;
    pNewScript->RegisterSelf();
}