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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gruuls_lair.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1565010,
    SAY_SLAM1                   = -1565011,
    SAY_SLAM2                   = -1565012,
    SAY_SHATTER1                = -1565013,
    SAY_SHATTER2                = -1565014,
    SAY_SLAY1                   = 20121,
    SAY_SLAY2                   = 20122,
    SAY_SLAY3                   = 20123,
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

struct boss_gruulAI : public CombatAI
{
    boss_gruulAI(Creature* creature) : CombatAI(creature, GRUUL_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(GRUUL_ACTION_SHATTER, false);
        AddCombatAction(GRUUL_ACTION_GROW, 30000u);
        AddCombatAction(GRUUL_ACTION_CAVE_IN, 10000u);
        AddCombatAction(GRUUL_ACTION_REVERBERATION, 115000u);
        AddCombatAction(GRUUL_ACTION_GROUND_SLAM, 35000, 40000);
        AddCombatAction(GRUUL_ACTION_HURTFUL_STRIKE, 6000u);
        AddOnKillText(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3);
    }

    ScriptedInstance* m_instance;

    bool m_lookAround;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatMovement(true);
        SetMeleeEnabled(true);
    }

    void Aggro(Unit* /*who*/) override
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

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_GRUUL_EVENT, DONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GRUUL_ACTION_SHATTER:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SHATTER) == CAST_OK)
                {
                    DoScriptText(EMOTE_SLAM, m_creature);
                    DoScriptText(urand(0, 1) ? SAY_SHATTER1 : SAY_SHATTER2, m_creature);
                    SetActionReadyStatus(action, false);
                    return;
                }
                return;
            }
            case GRUUL_ACTION_GROW:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_GROWTH) == CAST_OK)
                {
                    DoScriptText(EMOTE_GROW, m_creature);
                    ResetCombatAction(action, 30000);
                    return;
                }
                return;
            }
            case GRUUL_ACTION_GROUND_SLAM:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_GROUND_SLAM_DUMMY) == CAST_OK)
                {
                    m_creature->CastSpell(nullptr, SPELL_GROUND_SLAM, TRIGGERED_NONE);
                    DoScriptText(urand(0, 1) ? SAY_SLAM1 : SAY_SLAM2, m_creature);
                    m_creature->SetTarget(nullptr);
                    ResetCombatAction(action, urand(70000, 80000));
                    SetActionReadyStatus(GRUUL_ACTION_SHATTER, true); // top priority, blocked by stun
                    m_lookAround = true;
                    return;
                }
                return;
            }
            case GRUUL_ACTION_REVERBERATION:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_REVERBERATION) == CAST_OK)
                    ResetCombatAction(action, urand(35000, 45000));
                return;
            }
            case GRUUL_ACTION_CAVE_IN:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CAVE_IN) == CAST_OK)
                        ResetCombatAction(action, 8500);
                return;
            }
            case GRUUL_ACTION_HURTFUL_STRIKE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_HURTFUL_STRIKE) == CAST_OK)
                    ResetCombatAction(action, 8000);
                break;
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
};

struct GronnLordsGrasp : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            if (aura->GetStackAmount() >= 5 && !aura->GetTarget()->HasAura(33652))
                aura->GetTarget()->CastSpell(nullptr, 33652, TRIGGERED_OLD_TRIGGERED);
    }
};

struct HurtfulStrikePrimer : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetTypeId() != TYPEID_PLAYER || !spell->GetCaster()->CanReachWithMeleeAttack(target))
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        auto& targetInfo = spell->GetTargetList();
        if (!target || targetInfo.rbegin()->targetGUID != target->GetObjectGuid())
            return;

        for (auto& targetInfo : targetInfo)
        {
            if (caster->GetMap()->GetPlayer(targetInfo.targetGUID) == caster->GetVictim())
                continue;

            if (caster->getThreatManager().getThreat(caster->GetMap()->GetPlayer(targetInfo.targetGUID)) > caster->getThreatManager().getThreat(target) || target == caster->GetVictim())
                target = caster->GetMap()->GetPlayer(targetInfo.targetGUID);
        }

        caster->CastSpell(target, 33813, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_NORMAL_COMBAT_CAST);
    }
};

void AddSC_boss_gruul()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gruul";
    pNewScript->GetAI = &GetNewAIInstance<boss_gruulAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GronnLordsGrasp>("spell_gronn_lords_grasp");
    RegisterSpellScript<HurtfulStrikePrimer>("spell_hurtful_strike_primer");
}
