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
SDName: Boss_Sladran
SD%Complete: 75%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                 = 31711,
    SAY_SUMMON_SNAKE          = 30369,
    SAY_SUMMON_CONSTRICTOR    = 30370,
    SAY_SLAY_1                = 31712,
    SAY_SLAY_2                = 31713,
    SAY_SLAY_3                = 31714,
    SAY_DEATH                 = 31715,
    EMOTE_NOVA                = 30757,

    // Slad'Ran spells
    SPELL_POISON_NOVA         = 55081,
    SPELL_POISON_NOVA_H       = 59842,
    SPELL_POWERFUL_BITE       = 48287,
    SPELL_POWERFUL_BITE_H     = 59840,
    SPELL_VENOM_BOLT          = 54970,
    SPELL_VENOM_BOLT_H        = 59839,

    // Summon spells
    SPELL_SUMMON_VIPER        = 55060,
    SPELL_SUMMON_CONSTRICTOR  = 54969,

    // Constrictor spells
    SPELL_GRIP_OF_SLADRAN     = 55093,
    SPELL_GRIP_OF_SLADRAN_H   = 61474,

    // Snake Wrap spells - mechanics unk
    SPELL_SNAKE_WRAP          = 55099,
    SPELL_SNAKE_WRAP_H        = 61475,
    SPELL_SNAKE_WRAP_SUMMON   = 55126,
    SPELL_SNAKE_WRAP_SUMMON_H = 61476,
    SPELL_SNAKE_WRAP_EFFECT   = 55128,
    SPELL_SNAKE_WRAP_SNAKES   = 55127,              // kills all snakes

    NPC_SLADRAN_CONSTRICTOR   = 29713,
    NPC_SLADRAN_VIPER         = 29680,
    NPC_SNAKE_WRAP            = 29742,
};

/*######
## boss_sladran
######*/

enum SladranActions
{
    SLADRAN_SUMMON,
    SLADRAN_POISON_NOVA,
    SLADRAN_POWERFUL_BITE,
    SLADRAN_VENOM_BOLT,
    SLADRAN_ACTIONS_MAX,
};

struct boss_sladranAI : public BossAI
{
    boss_sladranAI(Creature* creature) : BossAI(creature, SLADRAN_ACTIONS_MAX),
    instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
    isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_SLADRAN);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddOnDeathText(SAY_DEATH);
        AddCombatAction(SLADRAN_SUMMON, isRegularMode ? 5s: 3s);
        AddCombatAction(SLADRAN_POISON_NOVA, 22s);
        AddCombatAction(SLADRAN_POWERFUL_BITE, 10s);
        AddCombatAction(SLADRAN_VENOM_BOLT, 15s);
    }

    instance_gundrak* instance;
    bool isRegularMode;

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_SLADRAN_CONSTRICTOR && summoned->GetEntry() != NPC_SLADRAN_VIPER)
            return;

        summoned->SetWalk(false);
        summoned->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
    }

    std::chrono::milliseconds GetSubsequentActionTimer(SladranActions action)
    {
        switch (action)
        {
            case SLADRAN_POISON_NOVA: return 22s;
            case SLADRAN_SUMMON: return isRegularMode ? 5s : 3s;
            case SLADRAN_POWERFUL_BITE: return 10s;
            case SLADRAN_VENOM_BOLT: return 15s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SLADRAN_POISON_NOVA:
            {
                if (DoCastSpellIfCan(m_creature, isRegularMode ? SPELL_POISON_NOVA : SPELL_POISON_NOVA_H) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_NOVA, m_creature);
                    break;
                }
                return;
            }
            case SLADRAN_SUMMON:
            {
                if (!instance)
                    return;

                if (Creature* summonTarget = m_creature->GetMap()->GetCreature(instance->SelectRandomSladranTargetGuid()))
                {
                    if (urand(0, 3))
                    {
                        // we don't want to get spammed
                        if (!urand(0, 4))
                            DoBroadcastText(SAY_SUMMON_CONSTRICTOR, m_creature);

                        summonTarget->CastSpell(summonTarget, SPELL_SUMMON_CONSTRICTOR, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                    }
                    else
                    {
                        // we don't want to get spammed
                        if (!urand(0, 4))
                            DoBroadcastText(SAY_SUMMON_SNAKE, m_creature);

                        summonTarget->CastSpell(summonTarget, SPELL_SUMMON_VIPER, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                    }
                }
                break;
            }
            case SLADRAN_POWERFUL_BITE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), isRegularMode ? SPELL_POWERFUL_BITE : SPELL_POWERFUL_BITE_H) == CAST_OK)
                    break;
                return;
            }
            case SLADRAN_VENOM_BOLT:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, isRegularMode ? SPELL_VENOM_BOLT : SPELL_VENOM_BOLT_H) == CAST_OK)
                        break;
                }
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(SladranActions(action)));
    }
};

void AddSC_boss_sladran()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sladran";
    pNewScript->GetAI = &GetNewAIInstance<boss_sladranAI>;
    pNewScript->RegisterSelf();
}
