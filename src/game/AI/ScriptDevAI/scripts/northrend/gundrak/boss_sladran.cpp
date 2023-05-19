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
    SLADRAN_HEALTH_CHECK,
    SLADRAN_ACTIONS_MAX,
};

struct boss_sladranAI : public BossAI
{
    boss_sladranAI(Creature* creature) : BossAI(creature, SLADRAN_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_SLADRAN);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddOnDeathText(SAY_DEATH);
        AddCombatAction(SLADRAN_SUMMON, true);
        AddTimerlessCombatAction(SLADRAN_HEALTH_CHECK, true);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_SLADRAN_CONSTRICTOR && summoned->GetEntry() != NPC_SLADRAN_VIPER)
            return;

        summoned->SetWalk(false);

        if (summoned->GetEntry() == NPC_SLADRAN_VIPER)
        {
            summoned->SetInCombatWithZone();
            return;
        }

        if (summoned->AI())
            summoned->AI()->SetReactState(REACT_DEFENSIVE);

        summoned->SetInCombatWithZone();
        summoned->AI()->AttackClosestEnemy();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(SladranActions action)
    {
        switch (action)
        {
            case SLADRAN_SUMMON: return m_isRegularMode ? 5s : 3s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SLADRAN_HEALTH_CHECK:
            {
                if (m_creature->GetHealthPercent() <= 90.f)
                {
                    ResetCombatAction(SLADRAN_SUMMON, 0s);
                    DisableCombatAction(SLADRAN_HEALTH_CHECK);
                }
                return;
            }
            case SLADRAN_SUMMON:
            {
                if (!m_instance)
                    return;

                if (Creature* summonTarget = m_creature->GetMap()->GetCreature(m_instance->SelectRandomSladranTargetGuid()))
                {
                    if (summonTarget->GetPositionZ() > 132)
                    {
                        // we don't want to get spammed
                        if (!urand(0, 4))
                            DoBroadcastText(SAY_SUMMON_CONSTRICTOR, m_creature);

                        summonTarget->CastSpell(nullptr, SPELL_SUMMON_CONSTRICTOR, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                    }
                    else
                    {
                        // we don't want to get spammed
                        if (!urand(0, 4))
                            DoBroadcastText(SAY_SUMMON_SNAKE, m_creature);

                        summonTarget->CastSpell(nullptr, SPELL_SUMMON_VIPER, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                    }
                }
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(SladranActions(action)));
    }
};

struct npc_snakeWrapAI : public Scripted_NoMovementAI
{
    npc_snakeWrapAI(Creature* creature) : Scripted_NoMovementAI(creature),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetReactState(REACT_PASSIVE);
        SetAIImmobilizedState(true);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;

    void JustDied(Unit* /*killer*/) override
    {
        Player* spawner = dynamic_cast<Player*>(m_creature->GetSpawner());
        if (spawner && spawner->IsAlive())
            spawner->RemoveAurasDueToSpell(m_isRegularMode ? SPELL_SNAKE_WRAP_SUMMON : SPELL_SNAKE_WRAP_SUMMON_H);
        m_creature->ForcedDespawn(std::chrono::milliseconds(1s).count());
    }
};

struct GripOfSladran : public AuraScript, public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(SPELL_SNAKE_WRAP_SUMMON) || target->HasAura(SPELL_SNAKE_WRAP_SUMMON_H))
            return false;
        return true;
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Player* target = dynamic_cast<Player*>(aura->GetTarget());
        if (!target)
            return;

        bool isRegularMode = target->GetMap()->IsRegularDifficulty();
        if (aura->GetStackAmount() == 5)
        {
            target->CastSpell(nullptr, isRegularMode ? SPELL_SNAKE_WRAP : SPELL_SNAKE_WRAP_H, TRIGGERED_OLD_TRIGGERED);
            target->RemoveAurasDueToSpell(aura->GetSpellProto()->Id);
        }
    }
};

struct SnakeWrap : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        if (apply)
            return;

        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        Player* target = dynamic_cast<Player*>(aura->GetTarget());
        if (!target)
            return;

        bool isRegularMode = target->GetMap()->IsRegularDifficulty();
        target->CastSpell(nullptr, isRegularMode ? SPELL_SNAKE_WRAP_SUMMON : SPELL_SNAKE_WRAP_SUMMON_H, TRIGGERED_OLD_TRIGGERED);
        instance_gundrak* instance = dynamic_cast<instance_gundrak*>(target->GetMap()->GetInstanceData());
        if (!instance)
            return;

        instance->SetData(TYPE_ACHIEV_WHY_SNAKES, target->GetGUIDLow());
    }
};

struct SummonConstrictorSladran : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        // rough constant to force actual floor picking
        // confirmed src pos is on caster Z and dest pos is on floor Z
        spell->m_targets.m_destPos.z = spell->GetTrueCaster()->GetMap()->GetHeight(spell->GetTrueCaster()->GetPhaseMask(), spell->m_targets.m_destPos.x, spell->m_targets.m_destPos.y, spell->m_targets.m_destPos.z - 8.f);
    }
};

void AddSC_boss_sladran()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sladran";
    pNewScript->GetAI = &GetNewAIInstance<boss_sladranAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gundrak_snake_wrap";
    pNewScript->GetAI = &GetNewAIInstance<npc_snakeWrapAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GripOfSladran>("spell_grip_of_sladran");
    RegisterSpellScript<SnakeWrap>("spell_gundrak_snake_wrap");
    RegisterSpellScript<SummonConstrictorSladran>("spell_summon_constrictor_sladran");
}
