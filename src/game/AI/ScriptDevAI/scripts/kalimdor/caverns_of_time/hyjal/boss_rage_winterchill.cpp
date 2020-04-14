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
#include "hyjal.h"

enum
{
    SAY_ENTER       = -1534040,
    SAY_FROST_NOVA1 = -1534041,
    SAY_FROST_NOVA2 = -1534042,
    SAY_DND1        = -1534043,
    SAY_DND2        = -1534044,
    SAY_KILL1       = -1534045,
    SAY_KILL2       = -1534046,
    SAY_KILL3       = -1534047,
    SAY_DEATH       = -1534048,
    SAY_ENRAGE      = -1000004, // Generic Berserker Rage emote

    // abilities
    SPELL_ICEBOLT           = 31249, // TODO: figure out if DR should apply
    SPELL_FROST_NOVA        = 31250,
    SPELL_DEATH_AND_DECAY   = 31258,
    SPELL_FROST_ARMOR       = 31256,
    SPELL_ENRAGE            = 26662,
};

enum WinterchillActions
{
    WINTERCHILL_ACTION_FROST_ARMOR,
    WINTERCHILL_ACTION_ICEBOLT,
    WINTERCHILL_ACTION_FROST_NOVA,
    WINTERCHILL_ACTION_DEATH_AND_DECAY,
    WINTERCHILL_ACTION_ENRAGE,
    WINTERCHILL_ACTION_MAX,
};

struct boss_rage_winterchillAI : public ScriptedAI
{
    boss_rage_winterchillAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());
        m_actionTimers.insert({ WINTERCHILL_ACTION_FROST_ARMOR , 0 });
        m_actionTimers.insert({ WINTERCHILL_ACTION_ICEBOLT , 0 });
        m_actionTimers.insert({ WINTERCHILL_ACTION_FROST_NOVA , 0 });
        m_actionTimers.insert({ WINTERCHILL_ACTION_DEATH_AND_DECAY , 0 });
        m_actionTimers.insert({ WINTERCHILL_ACTION_ENRAGE , 0 });
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_actionReadyStatus[WINTERCHILL_ACTION_MAX];
    std::map<uint32, uint32> m_actionTimers;

    void Reset() override
    {
        for (auto& data : m_actionTimers)
            data.second = GetInitialActionTimer(data.first);

        for (uint32 i = 0; i < WINTERCHILL_ACTION_MAX; ++i)
            m_actionReadyStatus[i] = false;
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case WINTERCHILL_ACTION_FROST_ARMOR: return urand(18000, 24000);
            case WINTERCHILL_ACTION_ICEBOLT: return urand(5000, 9000);
            case WINTERCHILL_ACTION_FROST_NOVA: return urand(12000, 17000);
            case WINTERCHILL_ACTION_DEATH_AND_DECAY: return urand(21000, 28000);
            case WINTERCHILL_ACTION_ENRAGE: return 600000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case WINTERCHILL_ACTION_FROST_ARMOR: return urand(30000, 45000);
            case WINTERCHILL_ACTION_ICEBOLT: return urand(9000, 15000);
            case WINTERCHILL_ACTION_FROST_NOVA: return urand(19000, 26000);
            case WINTERCHILL_ACTION_DEATH_AND_DECAY: return 45000;
            case WINTERCHILL_ACTION_ENRAGE: return 300000;
            default: return 0; // never occurs but for compiler
        }
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoScriptText(SAY_ENTER, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_instance)
            m_instance->SetData(TYPE_WINTERCHILL, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        uint32 textId;
        switch (urand(0, 2))
        {
            case 0: textId = SAY_KILL1; break;
            case 1: textId = SAY_KILL2; break;
            case 2: textId = SAY_KILL3; break;
        }
        DoScriptText(textId, m_creature);
    }

    void UpdateTimers(const uint32 diff)
    {
        for (auto& data : m_actionTimers)
        {
            uint32 index = data.first;
            if (!m_actionReadyStatus[index])
            {
                if (data.second <= diff)
                {
                    data.second = 0;
                    m_actionReadyStatus[index] = true;
                }
                else
                    data.second -= diff;
            }
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < WINTERCHILL_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case WINTERCHILL_ACTION_FROST_ARMOR:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_FROST_ARMOR) == CAST_OK)
                        {
                            m_actionTimers[i] = GetSubsequentActionTimer(i);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case WINTERCHILL_ACTION_ICEBOLT:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ICEBOLT, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_ICEBOLT) == CAST_OK)
                            {
                                m_actionTimers[i] = GetSubsequentActionTimer(i);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case WINTERCHILL_ACTION_FROST_NOVA:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_FROST_NOVA, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_FROST_NOVA) == CAST_OK)
                            {
                                DoScriptText(urand(0, 1) ? SAY_FROST_NOVA1 : SAY_FROST_NOVA2, m_creature);
                                m_actionTimers[i] = GetSubsequentActionTimer(i);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case WINTERCHILL_ACTION_DEATH_AND_DECAY:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DEATH_AND_DECAY, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_DEATH_AND_DECAY) == CAST_OK)
                            {
                                DoScriptText(urand(0, 1) ? SAY_DND1 : SAY_DND2, m_creature);
                                m_actionTimers[i] = GetSubsequentActionTimer(i);
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case WINTERCHILL_ACTION_ENRAGE:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                        {
                            DoScriptText(SAY_ENRAGE, m_creature);
                            m_actionTimers[i] = GetSubsequentActionTimer(i);
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        UpdateTimers(diff);
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_rage_winterchill(Creature* pCreature)
{
    return new boss_rage_winterchillAI(pCreature);
}

void AddSC_boss_rage_winterchill()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_rage_winterchill";
    pNewScript->GetAI = &GetAI_boss_rage_winterchill;
    pNewScript->RegisterSelf();
}