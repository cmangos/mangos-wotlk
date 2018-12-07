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
SDName: Boss_Void_Reaver
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "the_eye.h"

enum
{
    SAY_AGGRO                   = -1550000,
    SAY_SLAY1                   = -1550001,
    SAY_SLAY2                   = -1550002,
    SAY_SLAY3                   = -1550003,
    SAY_DEATH                   = -1550004,
    SAY_POUNDING1               = -1550005,
    SAY_POUNDING2               = -1550006,

    SPELL_POUNDING              = 34162,
    SPELL_ARCANE_ORB_MISSILE    = 34172,
    SPELL_KNOCK_AWAY            = 25778,
    SPELL_BERSERK               = 26662,

    NPC_ARCANE_ORB_TARGET       = 19577,
};

struct boss_void_reaverAI : public ScriptedAI
{
    boss_void_reaverAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_PERIODIC_LEECH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_PERIODIC_MANA_LEECH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DISPEL, DISPEL_POISON, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_HEALTH_LEECH, true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiPoundingTimer;
    uint32 m_uiArcaneOrbTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiBerserkTimer;

    void Reset() override
    {
        m_uiPoundingTimer   = 12000;
        m_uiArcaneOrbTimer  = 3000;
        m_uiKnockAwayTimer  = 30000;
        m_uiBerserkTimer    = 10 * MINUTE * IN_MILLISECONDS;
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOIDREAVER, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOIDREAVER, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOIDREAVER, NOT_STARTED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Cast the Arcane Orb missile on the npc, not on player
        DoCastSpellIfCan(pSummoned, SPELL_ARCANE_ORB_MISSILE, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Pounding
        if (m_uiPoundingTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_POUNDING) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_POUNDING1 : SAY_POUNDING2, m_creature);
                m_uiPoundingTimer = 14000;
            }
        }
        else
            m_uiPoundingTimer -= uiDiff;

        // Arcane Orb
        if (m_uiArcaneOrbTimer < uiDiff)
        {
            // Search only for players which are not within 18 yards of the boss
            std::vector<Unit*> suitableTargets;
            ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

            for (auto itr : threatList)
            {
                if (Unit* pTarget = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
                {
                    if (pTarget->GetTypeId() == TYPEID_PLAYER && !pTarget->IsWithinDist3d(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 18.0f))
                        suitableTargets.push_back(pTarget);
                }
            }

            if (suitableTargets.empty())
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    m_creature->SummonCreature(NPC_ARCANE_ORB_TARGET, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 15000);
                    m_uiArcaneOrbTimer = 3000;
                }
            }
            else
            {
                Unit* pTarget = suitableTargets[urand(0, suitableTargets.size() - 1)];

                if (pTarget)
                    m_creature->SummonCreature(NPC_ARCANE_ORB_TARGET, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSPAWN_TIMED_DESPAWN, 15000);

                m_uiArcaneOrbTimer = 3000;
            }
        }
        else
            m_uiArcaneOrbTimer -= uiDiff;

        // Single Target knock back, reduces aggro
        if (m_uiKnockAwayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY) == CAST_OK)
                m_uiKnockAwayTimer = 30000;
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

UnitAI* GetAI_boss_void_reaver(Creature* pCreature)
{
    return new boss_void_reaverAI(pCreature);
}

void AddSC_boss_void_reaver()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_void_reaver";
    pNewScript->GetAI = &GetAI_boss_void_reaver;
    pNewScript->RegisterSelf();
}
