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
SDName: Boss_Thorngrin_The_Tender
SD%Complete: 99.9
SDComment:
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_INTRO       = 16961,
    SAY_DEATH       = 20019,
    SAY_PLAYER_KILL = 20014,
    SAY_SACRIFICE   = 20015,
    SAY_20_HP       = 20013,
    SAY_50_HP       = 20016,
    SAY_HELLFIRE_1  = 20017,
    SAY_HELLFIRE_2  = 20018,
    SAY_AGGRO       = 20012,

    SPELL_HELLFIRE      = 34659,
    SPELL_HELLFIRE_H    = 39131,
    SPELL_SACRIFICE     = 34661,
    SPELL_ENRAGE        = 34670,
};

enum ThorngrinActions
{
    THORNGRIN_20,
    THORNGRIN_50,
    THORNGRIN_ACTION_MAX,
};

struct boss_thorngrinAI : CombatAI
{
    boss_thorngrinAI(Creature* creature) : CombatAI(creature, THORNGRIN_ACTION_MAX), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_hasYelledIntro(false)
    {
        AddTimerlessCombatAction(THORNGRIN_20, true);
        AddTimerlessCombatAction(THORNGRIN_50, true);
        AddOnKillText(SAY_PLAYER_KILL);
    }

    bool m_isRegularMode;
    bool m_hasYelledIntro;

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasYelledIntro && who->IsPlayer() && !static_cast<Player*>(who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 75.0f) && m_creature->IsWithinLOSInMap(who))
        {
            DoBroadcastText(SAY_INTRO, m_creature);
            m_hasYelledIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SACRIFICE: DoBroadcastText(SAY_SACRIFICE, m_creature); break;
            case SPELL_HELLFIRE:
            case SPELL_HELLFIRE_H: DoBroadcastText(urand(0, 1) ? SAY_HELLFIRE_1 : SAY_HELLFIRE_2, m_creature); break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case THORNGRIN_20:
                if (m_creature->GetHealthPercent() <= 20.f)
                {
                    DoBroadcastText(SAY_20_HP, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case THORNGRIN_50:
                if (m_creature->GetHealthPercent() <= 50.f)
                {
                    DoBroadcastText(SAY_50_HP, m_creature);
                    DisableCombatAction(action);
                }
                break;
        }
    }
};

void AddSC_boss_thorngrin()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_thorngrin";
    pNewScript->GetAI = &GetNewAIInstance<boss_thorngrinAI>;
    pNewScript->RegisterSelf();
}