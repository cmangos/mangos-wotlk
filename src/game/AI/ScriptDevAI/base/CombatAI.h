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

#ifndef COMBAT_AI_H
#define COMBAT_AI_H

#include "Entities/Creature.h"
#include "AI/ScriptDevAI/include/sc_creature.h"

/*
    Small wrapper class that reduces the amount of code needed to use CombatActions and adds proper spell action reset functionality
*/
class CombatAI : public ScriptedAI
{
    public:
        CombatAI(Creature* creature, uint32 combatActions);

        void Reset() override;

        void HandleDelayedInstantAnimation(SpellEntry const* spellInfo) override;
        void HandleTargetRestoration();
        bool IsTargetingRestricted();
        void StopTargeting(bool state) { m_stopTargeting = state; }
        void OnTaunt() override;

        void AddOnKillText(int32 text);
        template<typename... Targs>
        void AddOnKillText(int32 value, Targs... fargs)
        {
            AddOnKillText(value);
            AddOnKillText(fargs...);
        }
        void KilledUnit(Unit* /*victim*/) override;

        // virtual void ExecuteAction(uint32 action) {}
    private:
        ObjectGuid m_storedTarget;

        std::vector<int32> m_onDeathTexts;
        bool m_onKillCooldown;

        bool m_stopTargeting;
};

class BossAI : public CombatAI
{
    public:
        BossAI(Creature* creature, uint32 combatActions) : CombatAI(creature, combatActions)
        {}
        /**
        * Adds one or more Broadcast Texts to possibly emit when Unit dies
        * This function is not called if JustDied is overridden. Add CombatAI::JustDied(); to your overriding function.
        * @param text Broadcast Text ID
        */
        void AddOnDeathText(uint32 text);
        template<typename... Targs>
        void AddOnDeathText(uint32 value, Targs... fargs)
        {
            AddOnDeathText(value);
            AddOnDeathText(fargs...);
        }
        /**
        * Adds one or more Broadcast Texts to possibly emit when Unit enters combat
        * This function is not called if Aggro is overridden. Add CombatAI::Aggro(); to your overriding function.
        * @param text Broadcast Text ID
        */
        void AddOnAggroText(uint32 text);
        template<typename... Targs>
        void AddOnAggroText(uint32 value, Targs... fargs)
        {
            AddOnAggroText(value);
            AddOnAggroText(fargs...);
        }

        void SetDataType(uint32 type) { m_instanceDataType = type; }

        void JustDied(Unit* killer = nullptr) override;
        void JustReachedHome() override;
        void Aggro(Unit* who = nullptr) override;

        std::chrono::seconds TimeSinceEncounterStart()
        {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_combatStartTimestamp);
        }
    private:
        std::vector<uint32> m_onKilledTexts;
        std::vector<uint32> m_onAggroTexts;

        uint32 m_instanceDataType = -1;

        std::chrono::steady_clock::time_point m_combatStartTimestamp;
};

#endif
