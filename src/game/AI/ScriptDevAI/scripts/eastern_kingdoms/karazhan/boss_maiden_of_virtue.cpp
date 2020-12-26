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
SDName: Boss_Maiden_of_Virtue
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO               = -1532018,
    SAY_SLAY1               = -1532019,
    SAY_SLAY2               = -1532020,
    SAY_SLAY3               = -1532021,
    SAY_REPENTANCE1         = -1532022,
    SAY_REPENTANCE2         = -1532023,
    SAY_DEATH               = -1532024,

    SPELL_REPENTANCE        = 29511,
    SPELL_HOLYFIRE          = 29522,
    SPELL_HOLYWRATH         = 32445,
    SPELL_HOLYGROUND        = 29512
};

enum MaidenOfVirtueActions
{
    MAIDEN_ACTION_REPENTANCE,
    MAIDEN_ACTION_HOLY_FIRE,
    MAIDEN_ACTION_HOLY_WRATH,
    MAIDEN_ACTION_HOLY_GROUND,
    MAIDEN_ACTION_MAX,
};

struct boss_maiden_of_virtueAI : public CombatAI
{
    boss_maiden_of_virtueAI(Creature* creature) : CombatAI(creature, MAIDEN_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(MAIDEN_ACTION_REPENTANCE, 42000, 44000);
        AddCombatAction(MAIDEN_ACTION_HOLY_FIRE, 8000, 14000);
        AddCombatAction(MAIDEN_ACTION_HOLY_WRATH, 15000, 25000);
        AddCombatAction(MAIDEN_ACTION_HOLY_GROUND, 2000u);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float /*z*/)
        {
            return y > -2071.f && x > -10924.f;
        });

        AddOnKillText(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3);
    }

    ScriptedInstance* m_instance;

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case MAIDEN_ACTION_REPENTANCE: return urand(28000, 36000);
            case MAIDEN_ACTION_HOLY_FIRE: return urand(12000, 20000);
            case MAIDEN_ACTION_HOLY_WRATH: return urand(25000, 35000);
            case MAIDEN_ACTION_HOLY_GROUND: return 2000;
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAIDEN_ACTION_REPENTANCE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_REPENTANCE) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_REPENTANCE1 : SAY_REPENTANCE2, m_creature);
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                }
                return;
            }
            case MAIDEN_ACTION_HOLY_FIRE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HOLYFIRE, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_HOLYFIRE) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MAIDEN_ACTION_HOLY_WRATH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HOLYWRATH, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_HOLYWRATH) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MAIDEN_ACTION_HOLY_GROUND:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_HOLYGROUND) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MAIDEN, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MAIDEN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAIDEN, FAIL);
    }
};

void AddSC_boss_maiden_of_virtue()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_maiden_of_virtue";
    pNewScript->GetAI = &GetNewAIInstance<boss_maiden_of_virtueAI>;
    pNewScript->RegisterSelf();
}
