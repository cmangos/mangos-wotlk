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
SDName: Boss_Watchkeeper_Gargolmar
SD%Complete: 95
SDComment: Research Overpower Usage
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_TAUNT                   = 13999,
    SAY_HEAL                    = 17498,
    SAY_SURGE                   = 17499,
    SAY_AGGRO_1                 = 17502,
    SAY_AGGRO_2                 = 17503,
    SAY_AGGRO_3                 = 17504,
    SAY_KILL_1                  = 17500,
    SAY_KILL_2                  = 17501,
    SOUND_DIE                   = 10336,

    SPELL_MORTAL_WOUND          = 30641,
    SPELL_MORTAL_WOUND_H        = 36814,
    SPELL_SURGE                 = 34645,
    SPELL_RETALIATION           = 22857,
    SPELL_OVERPOWER             = 32154,

    NPC_HELLFIRE_WATCHER = 17309,
};

enum GargolmarActions // order based on priority
{
    GARGOLMAR_ACTION_YELL_FOR_HEAL,
    GARGOLMAR_ACTION_MORTAL_WOUND,
    GARGOLMAR_ACTION_SURGE,
    GARGOLMAR_ACTION_RETALIATION,
    GARGOLMAR_ACTION_OVERPOWER,
    GARGOLMAR_ACTION_MAX
};

struct boss_watchkeeper_gargolmarAI : public CombatAI
{
    boss_watchkeeper_gargolmarAI(Creature* creature) : CombatAI(creature, GARGOLMAR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_HasTaunted(false)
    {
        AddTimerlessCombatAction(GARGOLMAR_ACTION_YELL_FOR_HEAL, true);
        AddCombatAction(GARGOLMAR_ACTION_MORTAL_WOUND, 0u);
        AddCombatAction(GARGOLMAR_ACTION_SURGE, 4800u);
        AddCombatAction(GARGOLMAR_ACTION_RETALIATION, 0u);
        AddCombatAction(GARGOLMAR_ACTION_OVERPOWER, 3600u, 14800u);
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;
    bool m_HasTaunted;

    uint32 GetSubsequentActionTimer(GargolmarActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case GARGOLMAR_ACTION_MORTAL_WOUND: return urand(6100, 14600);
                case GARGOLMAR_ACTION_SURGE: return urand(12100, 25500);
                case GARGOLMAR_ACTION_RETALIATION: return 30000;
                case GARGOLMAR_ACTION_OVERPOWER: return urand(18100, 33700);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case GARGOLMAR_ACTION_MORTAL_WOUND: return 15800;
                case GARGOLMAR_ACTION_SURGE: return urand(12100, 15700);
                case GARGOLMAR_ACTION_RETALIATION: return 30000;
                case GARGOLMAR_ACTION_OVERPOWER: return urand(18100, 33700);
                default: return 0;
            }
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_HasTaunted && m_creature->IsWithinDistInMap(who, 60.0f))
        {
            DoBroadcastText(SAY_TAUNT, m_creature);
            m_HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoPlaySoundToSet(m_creature, SOUND_DIE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GARGOLMAR_ACTION_YELL_FOR_HEAL:
                if (m_creature->GetHealthPercent() < 40.0f)
                {
                    DoBroadcastText(SAY_HEAL, m_creature);

                    CreatureList watcherList;
                    GetCreatureListWithEntryInGrid(watcherList, m_creature, NPC_HELLFIRE_WATCHER, 100.0f);
                    for (Creature* watcher : watcherList)
                    {
                        if (watcher->IsAlive())
                            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, watcher);
                    }

                    SetActionReadyStatus(action, false);
                }
                break;
            case GARGOLMAR_ACTION_MORTAL_WOUND:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_MORTAL_WOUND : SPELL_MORTAL_WOUND_H) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(GargolmarActions(action)));
                    return;
                }
                break;
            case GARGOLMAR_ACTION_SURGE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_FARTHEST_AWAY, 0, SPELL_SURGE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_SURGE) == CAST_OK)
                        DoBroadcastText(SAY_SURGE, m_creature);

                    ResetCombatAction(action, GetSubsequentActionTimer(GargolmarActions(action)));
                    return;
                }
                break;
            case GARGOLMAR_ACTION_RETALIATION:
                if (m_creature->GetHealthPercent() < 20.0f && DoCastSpellIfCan(nullptr, SPELL_RETALIATION) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(GargolmarActions(action)));
                    return;
                }
                break;
            case GARGOLMAR_ACTION_OVERPOWER:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_OVERPOWER) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(GargolmarActions(action)));
                    return;
                }
                break;
        }
    }
};

void AddSC_boss_watchkeeper_gargolmar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_watchkeeper_gargolmar";
    pNewScript->GetAI = &GetNewAIInstance<boss_watchkeeper_gargolmarAI>;
    pNewScript->RegisterSelf();
}
