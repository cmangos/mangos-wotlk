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
SDName: Boss_Keristrasza
SD%Complete: 100%
SDComment:
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1576016,
    SAY_CRYSTAL_NOVA            = -1576017,
    SAY_ENRAGE                  = -1576018,
    SAY_KILL                    = -1576019,
    SAY_DEATH                   = -1576020,

    MAX_INTENSE_COLD_STACK      = 2,            // the max allowed stacks for the achiev to pass

    SPELL_INTENSE_COLD          = 48094,
    SPELL_INTENSE_COLD_AURA     = 48095,        // used for Intense cold achiev

    SPELL_CRYSTALFIRE_BREATH    = 48096,
    SPELL_CRYSTALFIRE_BREATH_H  = 57091,

    SPELL_CRYSTALLIZE           = 48179,        // heroic
    SPELL_CRYSTAL_CHAINS        = 50997,        // normal

    SPELL_TAIL_SWEEP            = 50155,

    SPELL_ENRAGE                = 8599
};

enum KeristraszaActions
{
    KERISTRASZA_ACTION_TAIL_SWEEP,
    KERISTRASZA_ACTION_CRYSTALLIZE,
    KERISTRASZA_ACTION_CRYSTALFIRE_BREATH,
    KERISTRASZA_ACTION_ENRAGE,
    KERISTRASZA_ACTION_MAX,
};

/*######
## boss_keristrasza
######*/

struct boss_keristraszaAI : public CombatAI
{
    boss_keristraszaAI(Creature* creature) : CombatAI(creature, KERISTRASZA_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData()))
    {
        AddCombatAction(KERISTRASZA_ACTION_TAIL_SWEEP, 10000u);
        AddCombatAction(KERISTRASZA_ACTION_CRYSTALFIRE_BREATH, 15000u);
        AddCombatAction(KERISTRASZA_ACTION_CRYSTALLIZE, 18000u);
        AddTimerlessCombatAction(KERISTRASZA_ACTION_ENRAGE, true);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();

        if (!m_instance)
            return;

        if (m_creature->IsAlive())
        {
            if (m_instance->GetData(TYPE_KERISTRASZA) != SPECIAL)
                DoCastSpellIfCan(m_creature, SPELL_FROZEN_PRISON, CAST_TRIGGERED);
        }
    }

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_INTENSE_COLD, CAST_AURA_NOT_PRESENT);

        if (m_instance)
            m_instance->SetData(TYPE_KERISTRASZA, IN_PROGRESS);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_KERISTRASZA, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KERISTRASZA_ACTION_TAIL_SWEEP:
                if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SWEEP) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case KERISTRASZA_ACTION_CRYSTALFIRE_BREATH:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_CRYSTALFIRE_BREATH : SPELL_CRYSTALFIRE_BREATH_H) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case KERISTRASZA_ACTION_CRYSTALLIZE:
                // different spells for heroic and non heroic
                if (m_isRegularMode)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CRYSTAL_CHAINS, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_CRYSTAL_CHAINS) == CAST_OK)
                            ResetCombatAction(action, 25000);
                    }
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CRYSTALLIZE) == CAST_OK)
                    {
                        ResetCombatAction(action, 25000);
                        DoScriptText(SAY_CRYSTAL_NOVA, m_creature);
                    }

                }
                break;
            case KERISTRASZA_ACTION_ENRAGE:
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        SetActionReadyStatus(action, false);
                    }
                }
                break;
        }
    }
};

/*######
## spell_intense_cold_aura - 48095
######*/

struct spell_intense_cold_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        // check only for heroic
        if (target->GetMap()->IsRegularDifficulty())
            return;

        // check achiev
        if (aura->GetStackAmount() > MAX_INTENSE_COLD_STACK)
        {
            if (instance_nexus* pInstance = static_cast<instance_nexus*>(target->GetInstanceData()))
                pInstance->SetData(TYPE_INTENSE_COLD_FAILED, target->GetGUIDLow());
        }
    }
};

void AddSC_boss_keristrasza()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_keristrasza";
    pNewScript->GetAI = &GetNewAIInstance<boss_keristraszaAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_intense_cold_aura>("spell_intense_cold_aura");
}
