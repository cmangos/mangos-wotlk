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
SDName: Boss_Grobbulus
SD%Complete: 80
SDComment: Timer need more care; Spells of Adds (Posion Cloud) need Mangos Fixes, and further handling
SDCategory: Naxxramas
EndScriptData */

/*Poison Cloud 26590
Slime Spray 28157
Fallout slime 28218
Mutating Injection 28169
Enrages 26527*/

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    EMOTE_SPRAY_SLIME               = 32318,
    EMOTE_INJECTION                 = 32319,

    SPELL_SLIME_STREAM              = 28137,
    SPELL_MUTATING_INJECTION        = 28169,
    SPELL_EMBALMING_CLOUD           = 28322,
    SPELL_MUTAGEN_EXPLOSION         = 28206,
    SPELL_POISON_CLOUD              = 28240,
    SPELL_SLIME_SPRAY               = 28157,
    SPELL_SLIME_SPRAY_H             = 54364,
    SPELL_SUMMON_FALLOUT_SLIME      = 28218,
    SPELL_BERSERK                   = 26662,

    SPELL_DESPAWN_SUMMONS           = 30134,

    NPC_FALLOUT_SLIME               = 16290
};

enum GrobbulusActions
{
    GROBBULUS_SLIME_STREAM,
    GROBBULUS_CHECK_MELEE,
    GROBBULUS_SLIME_SPRAY,
    GROBBULUS_INJECTION,
    GROBBULUS_POISON_CLOUD,
    GROBBULUS_BERSERK,
    GROBBULUS_ACTION_MAX,
};

struct boss_grobbulusAI : public BossAI
{
    boss_grobbulusAI(Creature* creature) : BossAI(creature, GROBBULUS_ACTION_MAX),
    m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
    m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty()),
    m_berserkTimer(m_bIsRegularMode ? 12min : 9min)
    {
        SetDataType(TYPE_GROBBULUS);
        AddCombatAction(GROBBULUS_SLIME_STREAM, true);
        AddCombatAction(GROBBULUS_SLIME_SPRAY, 20s, 30s); //Spell List
        AddCombatAction(GROBBULUS_INJECTION, 13s);
        AddCombatAction(GROBBULUS_POISON_CLOUD, 20s, 25s); //Spell List
        AddCombatAction(GROBBULUS_BERSERK, m_berserkTimer);
        AddCombatAction(GROBBULUS_CHECK_MELEE, 5s);
    }

    ScriptedInstance* m_instance;
    bool m_bIsRegularMode;
    std::chrono::milliseconds m_berserkTimer;

    void SpellHitTarget(Unit* target, const SpellEntry* spell) override
    {
        // Summon a Fallout Slime for every player hit by Slime Spray
        if ((spell->Id == SPELL_SLIME_SPRAY || spell->Id == SPELL_SLIME_SPRAY_H) && target->GetTypeId() == TYPEID_PLAYER)
            m_creature->SummonCreature(NPC_FALLOUT_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10 * IN_MILLISECONDS);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            // Mutagen Injection is used more often when below 30% HP
            case GROBBULUS_INJECTION:
            {
                if (m_bIsRegularMode)
                    return RandomTimer(10s, 13s) - 5 * std::chrono::milliseconds((m_berserkTimer - TimeSinceEncounterStart()) / m_berserkTimer);
                else
                    return RandomTimer(10s, 13s) - 8 * std::chrono::milliseconds((m_berserkTimer - TimeSinceEncounterStart()) / m_berserkTimer);
            }
            case GROBBULUS_POISON_CLOUD: return 15s;
            case GROBBULUS_SLIME_SPRAY: return RandomTimer(20s, 30s);
            case GROBBULUS_CHECK_MELEE: return 1s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GROBBULUS_CHECK_MELEE:
            {
                if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                {
                    ResetCombatAction(GROBBULUS_SLIME_STREAM, 5s);
                    DisableCombatAction(action);
                    break;
                }
                ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case GROBBULUS_SLIME_STREAM:
            {
                if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                    DoCastSpellIfCan(m_creature, SPELL_SLIME_STREAM, CAST_TRIGGERED);

                ResetCombatAction(GROBBULUS_CHECK_MELEE, GetSubsequentActionTimer(GROBBULUS_CHECK_MELEE));
                DisableCombatAction(action);
                break;
            }
            case GROBBULUS_INJECTION:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MUTATING_INJECTION, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK))
                {
                    if (DoCastSpellIfCan(target, SPELL_MUTATING_INJECTION, CAST_TRIGGERED) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                }
                break;
            }
            case GROBBULUS_POISON_CLOUD:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_POISON_CLOUD) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case GROBBULUS_SLIME_SPRAY:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_SLIME_SPRAY : SPELL_SLIME_SPRAY_H) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                    DoBroadcastText(EMOTE_SPRAY_SLIME, m_creature);
                }
                break;
            }
            case GROBBULUS_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                break;
            }
        }
    }
};

struct MutatingInjection : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            if (!target)
                return;

            if (aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                // Embalming Cloud
                target->CastSpell(target, SPELL_EMBALMING_CLOUD, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
            else // Removed by dispel
                // Mutagen Explosion
                target->CastSpell(target, SPELL_MUTAGEN_EXPLOSION, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
            // Poison Cloud
            target->CastSpell(target, SPELL_POISON_CLOUD, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
        }
        else
        {
            Unit* target = aura->GetTarget();
            Unit* caster = aura->GetCaster();
            if (!target || caster)
                return;
            DoBroadcastText(EMOTE_INJECTION, caster, target);
        }
    }
};

void AddSC_boss_grobbulus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_grobbulus";
    pNewScript->GetAI = &GetNewAIInstance<boss_grobbulusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<MutatingInjection>("spell_grobbulus_mutating_injection");
}
