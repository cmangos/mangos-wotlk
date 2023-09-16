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
SDName: Boss Mechano-Lord Capacitus
SD%Complete:
SDComment:
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "mechanar.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                       = 19952,
    SAY_DEATH                       = 19957,
    SAY_ABILITY_USE_1               = 19955,
    SAY_ABILITY_USE_2               = 19956,
    SAY_PLAYER_KILL_1               = 19953,
    SAY_PLAYER_KILL_2               = 19954,

    SPELL_SUMMON_NETHER_CHARGE_NE   = 35153,
    SPELL_SUMMON_NETHER_CHARGE_NW   = 35904,
    SPELL_SUMMON_NETHER_CHARGE_SE   = 35905,
    SPELL_SUMMON_NETHER_CHARGE_SW   = 35906,

    SPELL_POLARITY_SHIFT            = 39096,
    SPELL_HEAD_CRACK                = 35161,
    SPELL_BERSERK                   = 26662,
    SPELL_REFLECTIVE_MAGIC_SHIELD   = 35158,
    SPELL_REFLECTIVE_DAMAGE_SHIELD  = 35159,

    SPELL_NETHER_CHARGE_PASSIVE     = 35150,
    SPELL_NETHER_CHARGE_PULSE       = 35151,
    SPELL_NETHER_CHARGE_TIMER       = 37670,
};

enum CapacitusActions
{
    CAPACITUS_ACTIONS_MAX,
    CAPACITUS_NETHER_CHARGE
};

struct boss_mechano_lord_capacitusAI : public CombatAI
{
    boss_mechano_lord_capacitusAI(Creature* creature) : CombatAI(creature, CAPACITUS_ACTIONS_MAX), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_PLAYER_KILL_1, SAY_PLAYER_KILL_2);
        if (m_isRegularMode)
            AddCustomAction(CAPACITUS_NETHER_CHARGE, 2000, 5000, [&]() { HandleSummonCharge(); }, TIMER_COMBAT_COMBAT);
        else
            AddCustomAction(CAPACITUS_NETHER_CHARGE, 9000, 11000, [&]() { HandleSummonCharge(); }, TIMER_COMBAT_COMBAT);
    }

    bool m_isRegularMode;
    std::vector<ObjectGuid> m_summons;

    void Reset() override
    {
        CombatAI::Reset();

        DespawnGuids(m_summons);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        DespawnGuids(m_summons);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_REFLECTIVE_DAMAGE_SHIELD:
            case SPELL_REFLECTIVE_MAGIC_SHIELD:
                DoBroadcastText(urand(0, 1) ? SAY_ABILITY_USE_1 : SAY_ABILITY_USE_2, m_creature);
                break;
        }
    }

    void HandleSummonCharge()
    {
        uint32 spellId;
        switch (urand(0, 3))
        {
            case 0: spellId = SPELL_SUMMON_NETHER_CHARGE_NE; break;
            case 1: spellId = SPELL_SUMMON_NETHER_CHARGE_NW; break;
            case 2: spellId = SPELL_SUMMON_NETHER_CHARGE_SE; break;
            case 3: spellId = SPELL_SUMMON_NETHER_CHARGE_SW; break;
        }
        m_creature->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED); // packets not sent
        ResetTimer(CAPACITUS_NETHER_CHARGE, m_isRegularMode ? urand(2000, 15000) : urand(2000, 20000));
    }
};

struct NetherChargeAI : public ScriptedAI
{
    NetherChargeAI(Creature* creature) : ScriptedAI(creature), m_stopMoving(false)
    {
        AddCustomAction(1, true, [&]()
        {
            MoveToRandomPoint();
        });
        AddCustomAction(2, 10000u, [&]()
        {
            m_stopMoving = true;
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
        });
        SetReactState(REACT_PASSIVE);
    }

    void Reset() override { }

    bool m_stopMoving;

    void MoveToRandomPoint()
    {
        if (m_stopMoving)
            return;
        float x, y, z;
        m_creature->GetPosition(x, y, z); // do some urand radius shenanigans to spawn it further and make it walk to go using doing X and Y yourself and using function in MAP to get proper Z
        float xR = x + urand(10, 20), yR = y + urand(10, 20), zR = z;
        m_creature->UpdateAllowedPositionZ(xR, yR, zR);
        m_creature->GetMotionMaster()->MovePoint(1, xR, yR, zR);
    }

    void MovementInform(uint32 motionType, uint32 /*data*/) override
    {
        if (motionType == POINT_MOTION_TYPE && !m_stopMoving)
            ResetTimer(1, urand(0, 2000));
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(nullptr, SPELL_NETHER_CHARGE_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_NETHER_CHARGE_TIMER, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        MoveToRandomPoint();
    }
};

struct NetherChargePassive : public AuraScript
{
    void OnApply(Aura* aura, bool /*apply*/) const
    {
        aura->ForcePeriodicity(2000);
    }

    void OnPeriodicTickEnd(Aura* aura) const
    {
        switch (aura->GetAuraTicks())
        {
            case 5:
            case 6:
            case 7:
                aura->GetTarget()->CastSpell(nullptr, SPELL_NETHER_CHARGE_PULSE, TRIGGERED_OLD_TRIGGERED);
                break;
            default: break;
        }
    }
};

struct NetherChargeTimer : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        if (!apply)
            aura->GetTarget()->CastSpell(nullptr, aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()], TRIGGERED_OLD_TRIGGERED);
    }
};

// 39096 - Polarity Shift
struct PolarityShiftCapacitus : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        unitTarget->RemoveAurasDueToSpell(39088);
        unitTarget->RemoveAurasDueToSpell(39091);
        uint64 scriptValue = 0;

        // 39088 39091
        switch (spell->GetScriptValue())
        {
            case 0: // first target random
                scriptValue = urand(0, 1) ? 39088 : 39091;
                spell->SetScriptValue(scriptValue);
                unitTarget->CastSpell(unitTarget, scriptValue, TRIGGERED_OLD_TRIGGERED);
                break;
            case 39088: // second target the other
                spell->SetScriptValue(0);
                unitTarget->CastSpell(unitTarget, 39091, TRIGGERED_OLD_TRIGGERED);
                break;
            case 39091: // second target the other
                spell->SetScriptValue(0);
                unitTarget->CastSpell(unitTarget, 39088, TRIGGERED_OLD_TRIGGERED);
                break;
            default:
                break;
        }
    }
};

void AddSC_boss_mechano_lord_capacitus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_mechano_lord_capacitus";
    pNewScript->GetAI = &GetNewAIInstance<boss_mechano_lord_capacitusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nether_charge";
    pNewScript->GetAI = &GetNewAIInstance<NetherChargeAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<NetherChargePassive>("spell_nether_charge_passive");
    RegisterSpellScript<NetherChargeTimer>("spell_nether_charge_timer");
    RegisterSpellScript<PolarityShiftCapacitus>("spell_polarity_shift_capacitus");
}
