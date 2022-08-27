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
SDName: Boss_Gluth
SD%Complete: 95
SDComment: Gluth should turn around to face the victim when he devours a Zombie
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    EMOTE_ZOMBIE                    = 12242,
    EMOTE_ZOMBIES                   = 12348,
    EMOTE_BOSS_GENERIC_ENRAGED      = 7798,
    EMOTE_DECIMATE                  = 32321,

    SPELL_DOUBLE_ATTACK             = 19818,
    SPELL_MORTALWOUND               = 54378,                // old vanilla spell was 25646,
    SPELL_DECIMATE                  = 28374,
    SPELL_DECIMATE_H                = 54426,
    SPELL_DECIMATE_DAMAGE           = 28375,
    SPELL_ENRAGE                    = 28371,
    SPELL_ENRAGE_H                  = 54427,
    SPELL_BERSERK                   = 26662,
    // SPELL_TERRIFYING_ROAR         = 29685,               // no longer used in 3.x.x
    SPELL_SUMMON_ZOMBIE_CHOW        = 28216,                // triggers 28217 every 6 secs
    SPELL_SUMMON_ZOMBIE_CHOW_TR     = 28217,
    SPELL_CALL_ALL_ZOMBIE_CHOW      = 29681,                // triggers 29682
    SPELL_ZOMBIE_CHOW_SEARCH        = 28235,                // triggers 28236 every 3 secs
    SPELL_ZOMBIE_CHOW_SEARCH_INSTAKILL_TARGETED = 28239,    // Add usage
    SPELL_ZOMBIE_CHOW_SEARCH_INSTAKILL_AOE = 28404,

    NPC_ZOMBIE_CHOW                 = 16360,                // old vanilla summoning spell 28217

    MAX_ZOMBIE_LOCATIONS            = 3,

    SPELLSET_10N                    = 1593201,
    SPELLSET_25N                    = 2941701,
};

enum GluthActions
{
    GLUTH_ACTIONS_MAX,
    GLUTH_SUMMON_DELAY,
};

struct boss_gluthAI : public BossAI
{
    boss_gluthAI(Creature* creature) : BossAI(creature, GLUTH_ACTIONS_MAX), 
        m_instance(static_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_GLUTH);
        AddCustomAction(GLUTH_SUMMON_DELAY, true, [&]()
        {
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_ZOMBIE_CHOW, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        });
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        BossAI::Reset();
        m_creature->RemoveAurasDueToSpell(SPELL_SUMMON_ZOMBIE_CHOW);
        m_creature->RemoveAurasDueToSpell(SPELL_ZOMBIE_CHOW_SEARCH);
        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);
        ResetTimer(GLUTH_SUMMON_DELAY, 5s);
        DoCastSpellIfCan(nullptr, SPELL_ZOMBIE_CHOW_SEARCH, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void KilledUnit(Unit* victim) override
    {
        // Restore 5% hp when killing a zombie
        if (victim->GetEntry() == NPC_ZOMBIE_CHOW)
            m_creature->SetHealth(m_creature->GetHealth() + m_creature->GetMaxHealth() * 0.05f);
    }
};

struct ZombieChowSearch : AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        Creature* caster = static_cast<Creature*>(aura->GetCaster());
        if (!caster)
            return;
        if (!caster->AI())
            return;
        CreatureList zombiesInRange;
        GetCreatureListWithEntryInGrid(zombiesInRange, caster, NPC_ZOMBIE_CHOW, 10.f);
        for (auto zombieItr = zombiesInRange.begin();zombieItr != zombiesInRange.end();)
        {
            if (!(*zombieItr) || !(*zombieItr)->IsAlive())
                zombieItr = zombiesInRange.erase(zombieItr);
            else
                zombieItr++;
        }

        if (zombiesInRange.empty())
            return;
        else if(zombiesInRange.size() == 1)
        {
            Creature* pZombie = zombiesInRange.front();
            if (!pZombie)
                return;
            caster->GetMotionMaster()->MoveCharge(*pZombie, 30.f, EVENT_CHARGE);
            caster->AI()->DoCastSpellIfCan(pZombie, SPELL_ZOMBIE_CHOW_SEARCH_INSTAKILL_TARGETED);
            DoBroadcastText(EMOTE_ZOMBIE, caster);
            return;
        }
        caster->CastSpell(nullptr, SPELL_ZOMBIE_CHOW_SEARCH_INSTAKILL_AOE, TRIGGERED_OLD_TRIGGERED);
        DoBroadcastText(EMOTE_ZOMBIES, caster);
    }
};

struct GluthDecimate : SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (target->IsPlayer() || target->IsControlledByPlayer())
            return true;
        if (target->GetEntry() == NPC_ZOMBIE_CHOW)
            return true;
        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;
        if (!unitTarget->IsAlive())
            return;
        int32 damage = unitTarget->GetHealth() - unitTarget->GetMaxHealth() * 0.05f;
        if (damage > 0)
            spell->GetCaster()->CastCustomSpell(unitTarget, SPELL_DECIMATE_DAMAGE, &damage, nullptr, nullptr, TRIGGERED_INSTANT_CAST);
        if (unitTarget->IsPlayer() || unitTarget->IsControlledByPlayer())
            return;
        if (!unitTarget->AI())
            return;
        unitTarget->AttackStop(true, true);
        unitTarget->AI()->SetReactState(REACT_PASSIVE);
        unitTarget->GetMotionMaster()->MovePoint(0, spell->GetCaster()->GetPosition(), FORCED_MOVEMENT_WALK);
    }
};

struct SummonZombieChow : AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.spellInfo = nullptr;
        Creature* caster = static_cast<Creature*>(aura->GetCaster());
        if (!caster)
            return;
        instance_naxxramas* instance = dynamic_cast<instance_naxxramas*>(caster->GetInstanceData());
        if (!instance)
            return;

        GuidVector gluthTriggers = instance->GetGluthTriggers();
        if (gluthTriggers.size() < 3)
            return;

        std::shuffle(gluthTriggers.begin(), gluthTriggers.end(), *GetRandomGenerator());

        if (caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
            gluthTriggers.resize(1);
        else
            gluthTriggers.resize(2);

        for (auto& guid : gluthTriggers)
        {
            if (Creature* triggerNpc = caster->GetMap()->GetCreature(guid))
                triggerNpc->CastSpell(triggerNpc, SPELL_SUMMON_ZOMBIE_CHOW_TR, TRIGGERED_INSTANT_CAST);
        }
    }
};

struct EatZombieChowAOE : SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetEntry() != NPC_ZOMBIE_CHOW)
            return false;
        return true;
    }
};

void AddSC_boss_gluth()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gluth";
    pNewScript->GetAI = &GetNewAIInstance<boss_gluthAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GluthDecimate>("spell_decimate");
    RegisterSpellScript<SummonZombieChow>("spell_summon_zombie_chow");
    RegisterSpellScript<ZombieChowSearch>("spell_zombie_chow_search");
    RegisterSpellScript<EatZombieChowAOE>("spell_zombie_chow_search_instakill_aoe");
}
