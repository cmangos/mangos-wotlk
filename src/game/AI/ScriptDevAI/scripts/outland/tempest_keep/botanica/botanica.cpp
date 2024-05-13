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
#include "botanica.h"

enum BotanicaActions
{
    BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_01,
    BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_02,
    BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_03,
    BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_04,
    BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_05,
    BOTANICA_ACTION_MAX,
};

instance_botanica::instance_botanica(Map* map) : ScriptedInstance(map)
{
    // trigger 1 -1.2472258, Y:87.49708, Z:-5.6631246 
    auto posCheckTrigger01 = [](Unit const* unit) -> bool { return unit->GetPositionY() > 87.147f; };
    // trigger 2 X: -0.8652489 Y: 142.86328 Z: -5.539602 O: 1.5715766
    auto posCheckTrigger02 = [](Unit const* unit) -> bool { return unit->GetPositionY() > 142.86f; };
    // trigger 3 X: -0.39172745 Y: 219.03831 Z: -5.54034 O: 1.5785537
    auto posCheckTrigger03 = [](Unit const* unit) -> bool { return unit->GetPositionY() > 219.038f; };
    // trigger 4 X: -8.046078 Y: 288.01355 Z: 0.2993633
    auto posCheckTrigger04 = [](Unit const* unit) -> bool { return unit->GetPositionX() > -8.046f && unit->GetPositionY() > 288.013f; };
    // trigger 5 X: -157.25734, Y: 498.22766,  Z:-17.824787, 
    auto posCheckTrigger05 = [](Unit const* unit) -> bool { return unit->GetPositionX() < -157.257f && unit->GetPositionY() < 498.22f; };

    auto successEvent01 = [&]()
    {
        auto m_bloodwarderGroup = instance->GetCreatures(THIRD_BLOODWARDER_STRING);
        if (m_bloodwarderGroup != nullptr)
        {
            for (Creature* creature : *m_bloodwarderGroup)
            {
                if (!creature->IsAlive())
                    continue;

                creature->GetMotionMaster()->MoveWaypoint();
            }
        }
    };

    auto successEvent02 = [&]()
    {
        auto m_bloodwarderGroup = instance->GetCreatures(FOURTH_BLOODWARDER_STRING);
        if (m_bloodwarderGroup != nullptr)
        {
            for (Creature* creature : *m_bloodwarderGroup)
            {
                if (!creature->IsAlive())
                    continue;

                creature->GetMotionMaster()->MoveWaypoint();
            }
        }
    };

    auto successEvent03 = [&]()
    {
        auto m_bloodwarderGroup = instance->GetCreatures(FIFTH_BLOODWARDER_STRING);
        if (m_bloodwarderGroup != nullptr)
        {
            for (Creature* creature : *m_bloodwarderGroup)
            {
                if (!creature->IsAlive())
                    continue;

                creature->GetMotionMaster()->MoveWaypoint();
            }
        }
    };

    auto successEvent04 = [&]()
    {
        auto m_bloodwarderGroup = instance->GetCreatures(SIX_BLOODWARDER_STRING);
        if (m_bloodwarderGroup != nullptr)
        {
            for (Creature* creature : *m_bloodwarderGroup)
            {
                if (!creature->IsAlive())
                    continue;

                creature->GetMotionMaster()->MoveWaypoint();
            }
        }
    };

    auto successEvent05 = [&]()
        {
            auto m_bloodwarderGroup = instance->GetCreatures(SEVEN_BLOODWARDER_STRING);
            if (m_bloodwarderGroup != nullptr)
            {
                for (Creature* creature : *m_bloodwarderGroup)
                {
                    if (!creature->IsAlive())
                        continue;

                    creature->GetMotionMaster()->MoveWaypoint();
                }
            }
        };
    AddInstanceEvent(BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_01, posCheckTrigger01, successEvent01);
    AddInstanceEvent(BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_02, posCheckTrigger02, successEvent02);
    AddInstanceEvent(BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_03, posCheckTrigger03, successEvent03);
    AddInstanceEvent(BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_04, posCheckTrigger04, successEvent04);
    AddInstanceEvent(BOTANICA_TRIGGER_BLOODWARDER_PROTECTOR_05, posCheckTrigger05, successEvent05);
}

void instance_botanica::AddInstanceEvent(uint32 id, std::function<bool(Unit const*)> check, std::function<void()> successEvent)
{
    AddCustomAction(id, false, [instance = this->instance, check = check, successEvent = successEvent, id, this]()
    {
        for (const auto& data : instance->GetPlayers())
        {
            if (data.getSource()->IsGameMaster())
                continue;

            if (check(data.getSource()))
            {
                successEvent();
                return;
            }
        }
        ResetTimer(id, 1000);
    });
}

void instance_botanica::Update(const uint32 diff)
{
    UpdateTimers(diff);
}
// 34799 ArcaneDevastation
enum Spells
{
    SPELL_ARCANE_RESONANCE = 34794,
};

struct ArcaneDevastation : public SpellScript
{
    void OnAfterHit(Spell* spell) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        target->RemoveAurasDueToSpell(SPELL_ARCANE_RESONANCE);
        return;
    }
};

void AddSC_instance_botanica()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_botanica";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_botanica>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ArcaneDevastation>("spell_arcane_devastation");
}
