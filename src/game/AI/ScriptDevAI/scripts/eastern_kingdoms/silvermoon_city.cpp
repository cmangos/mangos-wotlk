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
SDName: Silvermoon_City
SD%Complete: 0
SDComment: Placeholder
SDCategory: Silvermoon City
EndScriptData */

/* ContentData
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"

struct EventGenerator001TurnAI : public ScriptedAI
{
    EventGenerator001TurnAI(Creature* creature) : ScriptedAI(creature), m_ori(false)
    {
        m_creature->SetCanEnterCombat(false);
        AddCustomAction(1, 5000u, [&]()
        {
            std::vector<Creature*> const* creaturesLeft = m_creature->GetMap()->GetCreatures("SILVERMOON_GUARDIANS_TURN_1");
            if (creaturesLeft)
            {
                for (Creature* creature : *creaturesLeft)
                    if (!creature->IsInCombat())
                        creature->SetFacingTo(m_ori ? 5.585053443908691406f : 4.014257431030273437f);
            }
            std::vector<Creature*> const* creaturesRight = m_creature->GetMap()->GetCreatures("SILVERMOON_GUARDIANS_TURN_2");
            if (creaturesRight)
            {
                for (Creature* creature : *creaturesRight)
                    if (!creature->IsInCombat())
                        creature->SetFacingTo(m_ori ? 2.443460941314697265f : 4.014257431030273437f);
            }
            m_ori = !m_ori;
            ResetTimer(1, 180000);
        });
    }

    bool m_ori;
};

UnitAI* GetNewAIInstance(Creature* creature)
{
    if (creature->HasStringId("SILVERMOON_GUARDIANS_TURN_EVENT"))
        return new EventGenerator001TurnAI(creature);
    return nullptr;
}

void AddSC_silvermoon_city()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_event_generator_001";
    pNewScript->GetAI = &GetNewAIInstance<EventGenerator001TurnAI>;
    pNewScript->RegisterSelf();
}
