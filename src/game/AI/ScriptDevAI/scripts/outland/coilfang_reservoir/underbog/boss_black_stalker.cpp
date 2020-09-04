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
#include "AI/ScriptDevAI/base/CombatAI.h"
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

struct boss_black_stalkerAI : public CombatAI
{
    boss_black_stalkerAI(Creature* creature) : CombatAI(creature, BLACK_STALKER_ACTION_MAX), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(BLACK_STALKER_ACTION_LEVITATE, 15000u);
        AddCombatAction(BLACK_STALKER_ACTION_STATIC_CHARGE, 33200, 39200);
        AddCombatAction(BLACK_STALKER_ACTION_CHAIN_LIGHTNING, 0, 3000);
        if (!m_isRegularMode)
            AddCombatAction(BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER, 20000, 40000);
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
    }

    ObjectGuid m_suspensionGuid;

    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case BLACK_STALKER_ACTION_LEVITATE: return 20500;
            case BLACK_STALKER_ACTION_STATIC_CHARGE: return urand(14000, 16000);
            case BLACK_STALKER_ACTION_CHAIN_LIGHTNING: return urand(12000, 29500);
            case BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER: return urand(20000, 30000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BLACK_STALKER_ACTION_LEVITATE:
            {
                if (m_creature->getThreatManager().getThreatList().size() > 1)
                    if (DoCastSpellIfCan(nullptr, SPELL_LEVITATE) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case BLACK_STALKER_ACTION_STATIC_CHARGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STATIC_CHARGE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_STATIC_CHARGE) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case BLACK_STALKER_ACTION_CHAIN_LIGHTNING:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CHAIN_LIGHTNING, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case BLACK_STALKER_ACTION_SUMMON_SPORE_STRIDER:
            {
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_SPORE_STRIDER, CAST_TRIGGERED);
                ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
        }
    }
};

struct StalkerLevitate : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effIdx*/) const override
    {
        if (spell->GetCaster()->GetVictim() == target || !target->IsPlayer()) // skip tank and non-player
            return false;

        return true;
    }

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
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
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
            if (unitTarget->IsPlayer())
            {
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
    pNewScript->GetAI = &GetNewAIInstance<boss_black_stalkerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<StalkerLevitate>("spell_levitate");
    RegisterSpellScript<SomeoneGrabMe>("spell_someone_grab_me");
    RegisterSpellScript<MagneticPull>("spell_magnetic_pull");
}
