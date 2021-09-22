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
SDName: Dun_Morogh
SD%Complete: 0
SDComment: Placeholder
SDCategory: Dun Morogh
EndScriptData */

/* ContentData
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_eastern_kingdoms.h"

std::vector<uint32> belbiTexts = { 22170, 22171, 22172, 22173, 22174, 22175 };

struct npc_belbi_quikswitch : public ScriptedAI
{
    npc_belbi_quikswitch(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_bCanStartScript;
    uint32 m_uiScriptCooldownTimer;
    uint32 m_uiShyTimer;

    void Reset() override
    {
        m_uiScriptCooldownTimer = 0;
        m_uiShyTimer            = 0;
        m_bCanStartScript       = true;
    }

    void StartScript(Player* player)
    {
        m_bCanStartScript = false;
        m_uiScriptCooldownTimer = 30000;
        m_uiShyTimer = 3000;
        m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
        DoBroadcastText(belbiTexts[urand(0, belbiTexts.size() - 1)], m_creature, player);
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

        if (m_uiShyTimer)
        {
            if (m_uiShyTimer < diff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_SHY);
                m_uiShyTimer = 0;
            }
            else
                m_uiShyTimer -= diff;
        }
    }
};

bool AreaTrigger_at_belbi_quikswitch(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    if (player->IsGameMaster() || !player->IsAlive())
        return false;

    if (Creature* belbi = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(NPC_BELBI_QUIKSWITCH))
        if (npc_belbi_quikswitch* belbiAI = dynamic_cast<npc_belbi_quikswitch*>(belbi->AI()))
            if (belbiAI->m_bCanStartScript)
                belbiAI->StartScript(player);

    return true;
}

void AddSC_dun_morogh()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_belbi_quikswitch";
    pNewScript->GetAI = &GetNewAIInstance<npc_belbi_quikswitch>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_belbi_quikswitch";
    pNewScript->pAreaTrigger = &AreaTrigger_at_belbi_quikswitch;
    pNewScript->RegisterSelf();
}
