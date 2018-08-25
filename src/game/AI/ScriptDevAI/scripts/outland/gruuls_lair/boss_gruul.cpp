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
SDName: Boss_Gruul
SD%Complete: 60
SDComment: Ground Slam need further development (knock back effect and shatter effect must be added to mangos)
SDCategory: Gruul's Lair
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "gruuls_lair.h"

enum
{
    SAY_AGGRO                   = -1565010,
    SAY_SLAM1                   = -1565011,
    SAY_SLAM2                   = -1565012,
    SAY_SHATTER1                = -1565013,
    SAY_SHATTER2                = -1565014,
    SAY_SLAY1                   = -1565015,
    SAY_SLAY2                   = -1565016,
    SAY_SLAY3                   = -1565017,
    SAY_DEATH                   = -1565018,

    EMOTE_GROW                  = -1565019,
    EMOTE_SLAM                  = -1565020,

    SPELL_GROWTH                = 36300,
    SPELL_CAVE_IN               = 36240,
    SPELL_GROUND_SLAM_DUMMY     = 39188,
    SPELL_LOOK_AROUND           = 33965,
    SPELL_GROUND_SLAM           = 33525,                    // AoE Ground Slam applying Ground Slam to everyone with a script effect (most likely the knock back, we can code it to a set knockback)
    SPELL_REVERBERATION         = 36297,
    SPELL_SHATTER               = 33654,

    SPELL_SHATTER_EFFECT        = 33671,
    SPELL_HURTFUL_STRIKE        = 33812,                    // Finds target for 33813
    SPELL_STONED                = 33652,                    // Spell is self cast by target
};

struct boss_gruulAI : public ScriptedAI
{
    boss_gruulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_instance = static_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_instance;

    enum GruulActions
    {
        GRUUL_ACTION_SHATTER,
        GRUUL_ACTION_GROW,
        GRUUL_ACTION_GROUND_SLAM,
        GRUUL_ACTION_REVERBERATION,
        GRUUL_ACTION_CAVE_IN,
        GRUUL_ACTION_HURTFUL_STRIKE,
        GRUUL_ACTION_MAX,
    };

    bool m_actionReadyStatus[GRUUL_ACTION_MAX];
    std::unordered_map<uint32, uint32> m_timers;
    bool m_lookAround;

    void Reset() override
    {
        SetCombatMovement(true);

        for (bool& m_actionReadyStatu : m_actionReadyStatus)
            m_actionReadyStatu = false;

        m_timers[GRUUL_ACTION_GROW] = 30000;
        m_timers[GRUUL_ACTION_CAVE_IN] = 10000;
        m_timers[GRUUL_ACTION_REVERBERATION] = 60000 + 55000;
        m_timers[GRUUL_ACTION_GROUND_SLAM] = urand(35000,40000);
        m_timers[GRUUL_ACTION_HURTFUL_STRIKE] = 6000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_GRUUL_EVENT, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_GRUUL_EVENT, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_GRUUL_EVENT, DONE);
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < GRUUL_ACTION_MAX; ++i)
        {
            if (!m_actionReadyStatus[i])
                continue;

            switch (i)
            {
                case GRUUL_ACTION_SHATTER:
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SHATTER) == CAST_OK)
                    {
                        DoScriptText(EMOTE_SLAM, m_creature);
                        DoScriptText(urand(0, 1) ? SAY_SHATTER1 : SAY_SHATTER2, m_creature);

                        SetCombatMovement(true);
                        m_meleeEnabled = true;
                        if (m_creature->getVictim())
                            m_creature->MeleeAttackStart(m_creature->getVictim());
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    continue;
                }
                case GRUUL_ACTION_GROW:
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_GROWTH) == CAST_OK)
                    {
                        DoScriptText(EMOTE_GROW, m_creature);
                        m_timers[GRUUL_ACTION_GROW] = 30000;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    continue;
                }
                case GRUUL_ACTION_GROUND_SLAM:
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_GROUND_SLAM_DUMMY) == CAST_OK)
                    {
                        m_creature->CastSpell(nullptr, SPELL_GROUND_SLAM, TRIGGERED_NONE);
                        DoScriptText(urand(0, 1) ? SAY_SLAM1 : SAY_SLAM2, m_creature);
                        SetCombatMovement(false);
                        m_meleeEnabled = false;
                        m_creature->MeleeAttackStop(m_creature->getVictim());
                        m_creature->SetTarget(nullptr);
                        m_timers[GRUUL_ACTION_GROUND_SLAM] = urand(70000, 80000);
                        m_actionReadyStatus[i] = false;
                        m_actionReadyStatus[GRUUL_ACTION_SHATTER] = true; // top priority, blocked by stun
                        m_lookAround = true;
                        return;
                    }
                    continue;
                }
                case GRUUL_ACTION_REVERBERATION:
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_REVERBERATION) == CAST_OK)
                    {
                        m_timers[GRUUL_ACTION_REVERBERATION] = urand(35000, 45000);
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    continue;
                }
                case GRUUL_ACTION_CAVE_IN:
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(target, SPELL_CAVE_IN) == CAST_OK)
                        {
                            m_timers[GRUUL_ACTION_CAVE_IN] = 8500;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                    }
                    continue;
                }
                case GRUUL_ACTION_HURTFUL_STRIKE:
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_HURTFUL_STRIKE) == CAST_OK)
                    {
                        m_timers[GRUUL_ACTION_HURTFUL_STRIKE] = 8000;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                }
            }
        }
    }

    void SpellHitTarget(Unit* /*target*/, const SpellEntry* spell) override 
    {
        if (spell->Id == SPELL_GROUND_SLAM && m_lookAround)
        {
            m_lookAround = false;
            m_creature->CastSpell(nullptr, SPELL_LOOK_AROUND, TRIGGERED_NONE);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget())
            return;

        for (auto& data : m_timers)
        {
            if (!m_actionReadyStatus[data.first])
            {
                if (data.second <= uiDiff)
                {
                    m_actionReadyStatus[data.first] = true;
                    data.second = 0;
                }
                else data.second -= uiDiff;
            }
        }
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_gruul(Creature* pCreature)
{
    return new boss_gruulAI(pCreature);
}

void AddSC_boss_gruul()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gruul";
    pNewScript->GetAI = &GetAI_boss_gruul;
    pNewScript->RegisterSelf();
}
