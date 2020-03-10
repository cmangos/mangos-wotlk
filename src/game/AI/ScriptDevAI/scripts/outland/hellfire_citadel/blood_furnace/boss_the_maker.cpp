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
SDName: Boss_The_Maker
SD%Complete: 80
SDComment: Mind control no support
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "blood_furnace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO_1                 = -1542009,
    SAY_AGGRO_2                 = -1542010,
    SAY_AGGRO_3                 = -1542011,
    SAY_KILL_1                  = -1542012,
    SAY_KILL_2                  = -1542013,
    SAY_DIE                     = -1542014,

    SPELL_EXPLODING_BREAKER     = 30925,
    SPELL_EXPLODING_BREAKER_H   = 40059,
    SPELL_DOMINATION            = 30923
};

enum MakerActions
{
    MAKER_EXPLODING_BEAKER,
    MAKER_DOMINATION,
    MAKER_ACTION_MAX,
};

struct boss_the_makerAI : public CombatAI
{
    boss_the_makerAI(Creature* creature) : CombatAI(creature, MAKER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(MAKER_EXPLODING_BEAKER, 6000u);
        AddCombatAction(MAKER_DOMINATION, 20000u);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_THE_MAKER_EVENT, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_THE_MAKER_EVENT, FAIL);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DIE, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_THE_MAKER_EVENT, DONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAKER_EXPLODING_BEAKER:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_EXPLODING_BREAKER : SPELL_EXPLODING_BREAKER_H) == CAST_OK)
                    ResetCombatAction(action, urand(4000, 12000));
                break;
            case MAKER_DOMINATION:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_DOMINATION) == CAST_OK)
                        ResetCombatAction(action, urand(20000, 25000));
                break;
        }
    }
};

void AddSC_boss_the_maker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_the_maker";
    pNewScript->GetAI = &GetNewAIInstance<boss_the_makerAI>;
    pNewScript->RegisterSelf();
}
