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
SDName: boss_black_stalker
SD%Complete: 75
SDComment: Spore Strider should have Delayed Despawn on The Black Stalker Death
SDCategory: Coilfang Resevoir, Underbog
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "Spells/Scripts/SpellScript.h"

enum // order based on priority
{
    SPELL_SOMEONE_GRAB_ME       = 31702,
    SPELL_MAGNETIC_PULL         = 31703,
    SPELL_LEVITATE              = 31704, // triggers 31701 Levitation Pulse
    SPELL_STATIC_CHARGE         = 31715,
    SPELL_CHAIN_LIGHTNING       = 31717,
    SPELL_SUSPENSION            = 31719,
    SPELL_SUMMON_SPORE_STRIDER  = 38755,

    NPC_BLACK_STALKER           = 17882,
};

enum BlackStalkerActions
{
    BLACK_STALKER_ACTION_LEVITATE,
    BLACK_STALKER_ACTION_STATIC_CHARGE,
    BLACK_STALKER_ACTION_CHAIN_LIGHTNING,
    BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER,
    BLACK_STALKER_ACTION_MAX,
    BLACK_STALKER_ACTION_SUSPENSION,
};

struct boss_black_stalkerAI : public ScriptedAI, public CombatActions
{
    boss_black_stalkerAI(Creature* creature) : ScriptedAI(creature), CombatActions(BLACK_STALKER_ACTION_MAX)
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        AddCombatAction(BLACK_STALKER_ACTION_LEVITATE, 0u);
        AddCombatAction(BLACK_STALKER_ACTION_STATIC_CHARGE, 0u);
        AddCombatAction(BLACK_STALKER_ACTION_CHAIN_LIGHTNING, 0u);
        AddCombatAction(BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER, 0u);
        AddCustomAction(BLACK_STALKER_ACTION_SUSPENSION, true, [&]()
        {
            if (Player* player = m_creature->GetMap()->GetPlayer(m_suspensionGuid))
            {
                player->CastSpell(nullptr, SPELL_SUSPENSION, TRIGGERED_OLD_TRIGGERED);
            }
        });
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* /*unit*/, float x, float y, float /*z*/)->bool
        {
            return x < 100.0f || y < -30.0f;
        });
        Reset();
    }

    ObjectGuid m_suspensionGuid;

    bool m_isRegularMode;

    void Reset() override
    {
        for (uint32 i = 0; i < BLACK_STALKER_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(BLACK_STALKER_ACTION_LEVITATE, GetInitialActionTimer(BLACK_STALKER_ACTION_LEVITATE));
        ResetTimer(BLACK_STALKER_ACTION_STATIC_CHARGE, GetInitialActionTimer(BLACK_STALKER_ACTION_STATIC_CHARGE));
        ResetTimer(BLACK_STALKER_ACTION_CHAIN_LIGHTNING, GetInitialActionTimer(BLACK_STALKER_ACTION_CHAIN_LIGHTNING));
        ResetTimer(BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER, GetInitialActionTimer(BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER));

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case BLACK_STALKER_ACTION_LEVITATE: return 15000;
            case BLACK_STALKER_ACTION_STATIC_CHARGE: return urand(33200, 39200);
            case BLACK_STALKER_ACTION_CHAIN_LIGHTNING: return urand(0, 3000);
            case BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER: return urand(10000, 15000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case BLACK_STALKER_ACTION_LEVITATE: return 20500;
            case BLACK_STALKER_ACTION_STATIC_CHARGE: return urand(14000, 16000);
            case BLACK_STALKER_ACTION_CHAIN_LIGHTNING: return urand(12000, 29500);
            case BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER: return urand(10000, 15000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < BLACK_STALKER_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case BLACK_STALKER_ACTION_LEVITATE:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_LEVITATE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case BLACK_STALKER_ACTION_STATIC_CHARGE:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STATIC_CHARGE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_STATIC_CHARGE) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                            }
                        }
                        break;
                    }
                    case BLACK_STALKER_ACTION_CHAIN_LIGHTNING:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CHAIN_LIGHTNING, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                            }
                        }
                        break;
                    }
                    case BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER:
                    {
                        if (!m_isRegularMode)
                        {
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_black_stalker(Creature* pCreature)
{
    return new boss_black_stalkerAI(pCreature);
}

struct Levitate : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            unitTarget->CastSpell(nullptr, SPELL_SOMEONE_GRAB_ME, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct SomeoneGrabMe : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            unitTarget->CastSpell(spell->GetCaster(), SPELL_MAGNETIC_PULL, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct MagneticPull : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            if (unitTarget->GetTypeId() == TYPEID_PLAYER)
            {
                //((Player*)unitTarget)->KnockBackFrom(spell->GetCaster(), 30.f, frand(28.58f, 30.65f));

                if (ScriptedInstance* pInstance = static_cast<ScriptedInstance*>(unitTarget->GetInstanceData()))
                {
                    if (Creature* boss = pInstance->GetSingleCreatureFromStorage(NPC_BLACK_STALKER))
                    {
                        if (boss_black_stalkerAI* stalkerAI = dynamic_cast<boss_black_stalkerAI*>(boss->AI()))
                        {
                            stalkerAI->m_suspensionGuid = unitTarget->GetObjectGuid();
                            stalkerAI->ResetTimer(BLACK_STALKER_ACTION_SUSPENSION, 1000);
                        }
                    }
                }
            }
        }
    }
};

void AddSC_boss_black_stalker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_black_stalker";
    pNewScript->GetAI = &GetAI_boss_black_stalker;
    pNewScript->RegisterSelf();

    RegisterSpellScript<Levitate>("spell_levitate");
    RegisterSpellScript<SomeoneGrabMe>("spell_someone_grab_me");
    RegisterSpellScript<MagneticPull>("spell_magnetic_pull");
}
