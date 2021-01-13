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
SD%Complete: 95%
SDComment: timers tuning
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "Spells/SpellAuras.h"

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

/*######
## boss_keristrasza
######*/

struct boss_keristraszaAI : public ScriptedAI
{
    boss_keristraszaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_nexus*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_nexus* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiCrystalChainTimer;
    uint32 uiTailSweepTimer;
    uint32 uiCrystalfireBreathTimer;
    uint32 uiCrystallizeTimer;

    bool m_bIsEnraged;

    void Reset() override
    {
        uiCrystalChainTimer = 30000;
        uiTailSweepTimer = urand(5000, 7500);
        uiCrystalfireBreathTimer = urand(10000, 20000);
        uiCrystallizeTimer = urand(20000, 30000);

        m_bIsEnraged = false;

        if (!m_pInstance)
            return;

        if (m_creature->IsAlive())
        {
            if (m_pInstance->GetData(TYPE_KERISTRASZA) != SPECIAL)
                DoCastSpellIfCan(m_creature, SPELL_FROZEN_PRISON, CAST_TRIGGERED);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_INTENSE_COLD, CAST_AURA_NOT_PRESENT);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KERISTRASZA, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KERISTRASZA, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bIsEnraged && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
            {
                DoScriptText(SAY_ENRAGE, m_creature);
                m_bIsEnraged = true;
            }
        }

        if (uiCrystalChainTimer < uiDiff)
        {
            // different spells for heroic and non heroic
            if (m_bIsRegularMode)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CRYSTAL_CHAINS, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CRYSTAL_CHAINS) == CAST_OK)
                        uiCrystalChainTimer = 30000;
                }
            }
            else
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CRYSTALLIZE) == CAST_OK)
                {
                    uiCrystalChainTimer = urand(15000, 25000);
                    DoScriptText(SAY_CRYSTAL_NOVA, m_creature);
                }

            }
        }
        else
            uiCrystalChainTimer -= uiDiff;

        if (uiTailSweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SWEEP) == CAST_OK)
                uiTailSweepTimer = urand(2500, 7500);
        }
        else
            uiTailSweepTimer -= uiDiff;

        if (uiCrystalfireBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CRYSTALFIRE_BREATH : SPELL_CRYSTALFIRE_BREATH_H) == CAST_OK)
                uiCrystalfireBreathTimer = urand(15000, 20000);
        }
        else
            uiCrystalfireBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
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

    RegisterAuraScript<spell_intense_cold_aura>("spell_intense_cold_aura");
}
