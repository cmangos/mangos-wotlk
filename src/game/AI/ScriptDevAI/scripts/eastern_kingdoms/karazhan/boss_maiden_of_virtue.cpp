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

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

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

enum CuratorActions
{
    MAIDEN_ACTION_REPENTANCE,
    MAIDEN_ACTION_HOLY_FIRE,
    MAIDEN_ACTION_HOLY_WRATH,
    MAIDEN_ACTION_HOLY_GROUND,
    MAIDEN_ACTION_MAX,
};

struct boss_maiden_of_virtueAI : public ScriptedAI, public CombatTimerAI
{
    boss_maiden_of_virtueAI(Creature* pCreature) : ScriptedAI(pCreature), CombatTimerAI(MAIDEN_ACTION_MAX)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        AddCombatAction(MAIDEN_ACTION_REPENTANCE, 0);
        AddCombatAction(MAIDEN_ACTION_HOLY_FIRE, 0);
        AddCombatAction(MAIDEN_ACTION_HOLY_WRATH, 0);
        AddCombatAction(MAIDEN_ACTION_HOLY_GROUND, 0);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override
    {
        for (uint32 i = 0; i < MAIDEN_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(MAIDEN_ACTION_REPENTANCE, GetInitialActionTimer(MAIDEN_ACTION_REPENTANCE));
        ResetTimer(MAIDEN_ACTION_HOLY_FIRE, GetInitialActionTimer(MAIDEN_ACTION_HOLY_FIRE));
        ResetTimer(MAIDEN_ACTION_HOLY_WRATH, GetInitialActionTimer(MAIDEN_ACTION_HOLY_WRATH));
        ResetTimer(MAIDEN_ACTION_HOLY_GROUND, GetInitialActionTimer(MAIDEN_ACTION_HOLY_GROUND));
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case MAIDEN_ACTION_REPENTANCE: return urand(28000, 32000);
            case MAIDEN_ACTION_HOLY_FIRE: return urand(8000, 14000);
            case MAIDEN_ACTION_HOLY_WRATH: return urand(15000, 25000);
            case MAIDEN_ACTION_HOLY_GROUND: return 2000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case MAIDEN_ACTION_REPENTANCE: return urand(28000, 32000);
            case MAIDEN_ACTION_HOLY_FIRE: return urand(12000, 20000);
            case MAIDEN_ACTION_HOLY_WRATH: return urand(25000, 35000);
            case MAIDEN_ACTION_HOLY_GROUND: return 2000;
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < MAIDEN_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case MAIDEN_ACTION_REPENTANCE:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_REPENTANCE);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MAIDEN_ACTION_HOLY_FIRE:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HOLYFIRE, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, SPELL_HOLYFIRE);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MAIDEN_ACTION_HOLY_WRATH:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, SPELL_HOLYWRATH);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MAIDEN_ACTION_HOLY_GROUND:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_HOLYGROUND, CAST_TRIGGERED);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                }
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 5)) // 50% chance to say something out of 3 texts
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        UpdateTimers(uiDiff, m_creature->isInCombat());
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_maiden_of_virtue(Creature* pCreature)
{
    return new boss_maiden_of_virtueAI(pCreature);
}

void AddSC_boss_maiden_of_virtue()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_maiden_of_virtue";
    pNewScript->GetAI = &GetAI_boss_maiden_of_virtue;
    pNewScript->RegisterSelf();
}
