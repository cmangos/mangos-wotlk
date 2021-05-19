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

enum
{
    SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES = 28784,
};

struct go_bonfire : GameObjectAI
{
    go_bonfire(GameObject* go) : GameObjectAI(go) {}

    void JustSpawned() override
    {
        m_go->CastSpell(nullptr, nullptr, SPELL_SUMMON_MIDSUMMER_BONFIRE_BUNNIES, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_midsummer_festival()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "go_midsummer_bonfire";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_bonfire>;
    pNewScript->RegisterSelf();
}