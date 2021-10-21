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
    SAY_AGGRO                       = -1554005,
    SAY_DEATH                       = -1554002,
    SAY_ABILITY_USE_1               = -1554000,
    SAY_ABILITY_USE_2               = -1554001,
    SAY_PLAYER_KILL_1               = -1554003,
    SAY_PLAYER_KILL_2               = -1554004,

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

struct boss_mechano_lord_capacitusAI : public ScriptedAI
{
    boss_mechano_lord_capacitusAI(Creature* creature) : ScriptedAI(creature), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        Reset();
    }

    bool m_isRegularMode;
    uint32 m_berserkTimer;
    uint32 m_polarityShiftTimer;
    uint32 m_headCrackTimer;
    uint32 m_netherChargeTimer;
    uint32 m_reflectiveShieldTimer;
    bool m_reflectiveShield;
    std::vector<ObjectGuid> m_summons;

    void Reset() override
    {
        m_reflectiveShield  = true;
        m_berserkTimer      = 180000;
        m_polarityShiftTimer = 30000;
        m_headCrackTimer = urand(16100, 18600);
        m_netherChargeTimer = m_isRegularMode ? urand(2000, 5000) : urand(9000, 11000);
        m_reflectiveShieldTimer = 15000;

        DespawnNetherCharges();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_PLAYER_KILL_1 : SAY_PLAYER_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnNetherCharges();
    }

    void DespawnNetherCharges()
    {
        for (ObjectGuid& guid : m_summons)
            if (Creature* charge = m_creature->GetMap()->GetCreature(guid))
                charge->ForcedDespawn();

        m_summons.clear();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_isRegularMode)
        {
            // Damage Shields
            if (m_reflectiveShieldTimer < diff)
            {
                if (m_reflectiveShield)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_REFLECTIVE_DAMAGE_SHIELD) == CAST_OK)
                    {
                        m_reflectiveShieldTimer = 20000;
                        m_reflectiveShield = false;
                    }
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_REFLECTIVE_MAGIC_SHIELD) == CAST_OK)
                    {
                        m_reflectiveShieldTimer = 20000;
                        m_reflectiveShield = true;
                    }
                }
            }
            else
                m_reflectiveShieldTimer -= diff;
        }
        else
        {
            // Polarity Shift
            if (m_polarityShiftTimer < diff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_POLARITY_SHIFT) == CAST_OK)
                    m_polarityShiftTimer = urand(27000, 34000);

                switch (urand(0, 3))
                {
                    case 0: DoScriptText(SAY_ABILITY_USE_1, m_creature); break;
                    case 1: DoScriptText(SAY_ABILITY_USE_2, m_creature); break;
                    default: break;
                }
            }
            else
                m_polarityShiftTimer -= diff;
        }

        // Berserk
        if (m_netherChargeTimer < diff)
        {
            uint32 spellId;
            switch (urand(0, 3))
            {
                case 0: spellId = SPELL_SUMMON_NETHER_CHARGE_NE; break;
                case 1: spellId = SPELL_SUMMON_NETHER_CHARGE_NW; break;
                case 2: spellId = SPELL_SUMMON_NETHER_CHARGE_SE; break;
                default:
                case 3: spellId = SPELL_SUMMON_NETHER_CHARGE_SW; break;
            }
            m_creature->CastSpell(m_creature, spellId, TRIGGERED_NONE);
            m_netherChargeTimer = m_isRegularMode ? urand(2000, 15000) : urand(2000, 20000);

            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_ABILITY_USE_1, m_creature); break;
                case 1: DoScriptText(SAY_ABILITY_USE_2, m_creature); break;
                default: break;
            }
        }
        else
            m_netherChargeTimer -= diff;

        // Head Crack
        if (m_headCrackTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEAD_CRACK) == CAST_OK)
                m_headCrackTimer = urand(19500, 33500);
        }
        else
            m_headCrackTimer -= diff;

        // Berserk
        if (m_berserkTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                m_berserkTimer = 300000;
        }
        else
            m_berserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct NetherCharge : public ScriptedAI
{
    NetherCharge(Creature* creature) : ScriptedAI(creature), m_stopMoving(false)
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

void AddSC_boss_mechano_lord_capacitus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_mechano_lord_capacitus";
    pNewScript->GetAI = &GetNewAIInstance<boss_mechano_lord_capacitusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nether_charge";
    pNewScript->GetAI = &GetNewAIInstance<NetherCharge>;
    pNewScript->RegisterSelf();

    RegisterAuraScript<NetherChargePassive>("spell_nether_charge_passive");
    RegisterAuraScript<NetherChargeTimer>("spell_nether_charge_timer");
}
