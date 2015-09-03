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
SDName: boss_eck
SD%Complete: 80%
SDComment: Timers need improval, Spring Spells are not clear.
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    SPELL_ECK_BITE            = 55813,
    SPELL_ECK_SPIT            = 55814,
    SPELL_ECK_SPRING          = 55815,
    SPELL_ECK_BERSERK         = 55816,
    SPELL_ECK_RESIDUE         = 55817,
    SPELL_ECK_SPRING_ALT      = 55837,                      // Purpose unknown
};

/*######
## boss_eck
######*/

struct boss_eckAI : public ScriptedAI
{
    boss_eckAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsBerserk;

    uint32 m_uiSpitTimer;
    uint32 m_uiSpringTimer;
    uint32 m_uiBiteTimer;
    uint32 m_uiBerserkTimer;

    void Reset() override
    {
        m_uiSpitTimer = urand(10000, 20000);
        m_uiSpringTimer = urand(15000, 25000);
        m_uiBiteTimer = urand(5000, 15000);
        m_uiBerserkTimer = urand(60000, 90000);             // Enrange at 20% HP or after 60-90 seconds
        m_bIsBerserk = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ECK, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ECK, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ECK, FAIL);
    }

    // As the Eck Spite spell has no dummy or similar effect, applying the residue aura has to be done with spellHitTarget
    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_ECK_SPIT && pUnit->GetTypeId() == TYPEID_PLAYER && !pUnit->HasAura(SPELL_ECK_RESIDUE))
            pUnit->CastSpell(pUnit, SPELL_ECK_RESIDUE, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpitTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ECK_SPIT) == CAST_OK)
                m_uiSpitTimer = urand(10000, 20000);
        }
        else
            m_uiSpitTimer -= uiDiff;

        if (m_uiSpringTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (DoCastSpellIfCan(pTarget, SPELL_ECK_SPRING) == CAST_OK)
            {
                DoResetThreat();
                m_uiSpringTimer = urand(15000, 25000);
            }
        }
        else
            m_uiSpringTimer -= uiDiff;

        if (m_uiBiteTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ECK_BITE);
            m_uiBiteTimer = urand(5000, 15000);
        }
        else
            m_uiBiteTimer -= uiDiff;

        if (!m_bIsBerserk)                                  // Go into Berserk after time, or when below 20% health
        {
            if (m_creature->GetHealthPercent() <= 20.0f || m_uiBerserkTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ECK_BERSERK) == CAST_OK)
                    m_bIsBerserk = true;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eck(Creature* pCreature)
{
    return new boss_eckAI(pCreature);
}

void AddSC_boss_eck()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_eck";
    pNewScript->GetAI = &GetAI_boss_eck;
    pNewScript->RegisterSelf();
}
