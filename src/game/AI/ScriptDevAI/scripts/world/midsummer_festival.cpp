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
#include "World/WorldState.h"

enum
{
    SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES = 28784,

    SPELL_STAMP_OUT_BONFIRE = 45437,
    SPELL_STAMP_OUT_BONFIRE_ART_KIT = 46903,

    SPELL_LIGHT_BONFIRE = 46904,
};

struct go_bonfire : GameObjectAI
{
    go_bonfire(GameObject* go) : GameObjectAI(go), m_state(true){ m_go->SetGoArtKit(121); }

    bool m_alliance;
    bool m_state;

    void JustSpawned() override
    {
        m_alliance = std::strcmp(m_go->GetGOInfo()->name, "Alliance Bonfire") == 0;
        m_go->CastSpell(nullptr, nullptr, SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES, TRIGGERED_OLD_TRIGGERED);
        sWorldState.SetBonfireZone(m_go->GetEntry(), m_go->GetZoneId(), m_alliance);
        sWorldState.SetBonfireActive(m_go->GetEntry(), m_alliance, true);
    }

    void ChangeState(bool active)
    {
        m_state = active;
        m_go->SetGoArtKit(active ? 121 : 122);
        sWorldState.SetBonfireActive(m_go->GetEntry(), m_alliance, active);
        m_go->SendGameObjectCustomAnim(m_go->GetObjectGuid());
    }

    void ReceiveAIEvent(AIEventType eventType, uint32 /*miscValue*/ = 0)
    {
        if (m_state && eventType == AI_EVENT_CUSTOM_A)
            ChangeState(false);
        else if (!m_state && eventType == AI_EVENT_CUSTOM_B)
            ChangeState(true);
    }
};

bool QuestRewardedBonfireDesecrate(Player* player, GameObject* go, Quest const* quest)
{
    player->CastSpell(nullptr, SPELL_STAMP_OUT_BONFIRE, TRIGGERED_NONE);
    player->CastSpell(nullptr, SPELL_STAMP_OUT_BONFIRE_ART_KIT, TRIGGERED_NONE);
    return true;
}

struct LightBonfire : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_LIGHT_BONFIRE, TRIGGERED_NONE);
    }
};

void AddSC_midsummer_festival()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "go_midsummer_bonfire";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_bonfire>;
    pNewScript->pQuestRewardedGO = &QuestRewardedBonfireDesecrate;
    pNewScript->RegisterSelf();

    RegisterSpellScript<LightBonfire>("spell_light_bonfire");
}