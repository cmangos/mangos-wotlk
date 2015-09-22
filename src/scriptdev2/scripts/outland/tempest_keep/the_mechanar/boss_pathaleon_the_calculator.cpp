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
SDName: Boss Pathaleon the Calculator
SD%Complete: 95
SDComment: Timers may need update.
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "precompiled.h"
#include "mechanar.h"

enum
{
    SAY_AGGRO                       = -1554020,
    SAY_DOMINATION_1                = -1554021,
    SAY_DOMINATION_2                = -1554022,
    SAY_SUMMON                      = -1554023,
    SAY_ENRAGE                      = -1554024,
    SAY_SLAY_1                      = -1554025,
    SAY_SLAY_2                      = -1554026,
    SAY_DEATH                       = -1554027,

    // Spells to be casted
    SPELL_MANA_TAP                  = 36021,
    SPELL_ARCANE_TORRENT            = 36022,
    SPELL_DOMINATION                = 35280,
    SPELL_ARCANE_EXPLOSION_H        = 15453,
    SPELL_FRENZY                    = 36992,
    SPELL_SUICIDE                   = 35301,        // kill the Nether Wraiths
    SPELL_DISGRUNTLED_ANGER         = 35289,        // empower a Nether Wraith

    SPELL_SUMMON_NETHER_WRAITH_1    = 35285,
    SPELL_SUMMON_NETHER_WRAITH_2    = 35286,
    SPELL_SUMMON_NETHER_WRAITH_3    = 35287,
    SPELL_SUMMON_NETHER_WRAITH_4    = 35288,

    // Add Spells
    SPELL_DETONATION                = 35058,
    SPELL_ARCANE_BOLT               = 20720,
};

static const uint32 aWraithSummonSpells[4] = {SPELL_SUMMON_NETHER_WRAITH_1, SPELL_SUMMON_NETHER_WRAITH_2, SPELL_SUMMON_NETHER_WRAITH_3, SPELL_SUMMON_NETHER_WRAITH_4};

struct boss_pathaleon_the_calculatorAI : public ScriptedAI
{
    boss_pathaleon_the_calculatorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiAngerTimer;
    uint32 m_uiManaTapTimer;
    uint32 m_uiArcaneTorrentTimer;
    uint32 m_uiDominationTimer;
    uint32 m_uiArcaneExplosionTimer;
    bool m_bIsEnraged;

    void Reset() override
    {
        m_uiSummonTimer          = urand(12000, 23000);
        m_uiAngerTimer           = urand(31000, 42000);
        m_uiManaTapTimer         = urand(2000, 9000);
        m_uiArcaneTorrentTimer   = urand(11000, 24000);
        m_uiDominationTimer      = urand(25000, 40000);
        m_uiArcaneExplosionTimer = urand(18000, 45000);
        m_bIsEnraged             = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATHALEON, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiManaTapTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_TAP, SELECT_FLAG_POWER_MANA))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MANA_TAP) == CAST_OK)
                    m_uiManaTapTimer = urand(16000, 34000);
            }
        }
        else
            m_uiManaTapTimer -= uiDiff;

        if (m_uiArcaneTorrentTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_TORRENT) == CAST_OK)
                m_uiArcaneTorrentTimer = urand(40000, 52000);
        }
        else
            m_uiArcaneTorrentTimer -= uiDiff;

        if (m_uiDominationTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DOMINATION) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_DOMINATION_1 : SAY_DOMINATION_2, m_creature);
                    m_uiDominationTimer = urand(25000, 30000);
                }
            }
        }
        else
            m_uiDominationTimer -= uiDiff;

        // Only casting if Heroic Mode is used
        if (!m_bIsRegularMode)
        {
            if (m_uiArcaneExplosionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_EXPLOSION_H) == CAST_OK)
                    m_uiArcaneExplosionTimer = urand(13000, 25000);
            }
            else
                m_uiArcaneExplosionTimer -= uiDiff;
        }

        if (!m_bIsEnraged && m_creature->GetHealthPercent() < 21.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_SUICIDE, CAST_TRIGGERED);
                DoScriptText(SAY_ENRAGE, m_creature);
                m_bIsEnraged = true;
            }
        }
        // Summon and empower Nether Wraiths only when not enraged
        else
        {
            if (m_uiSummonTimer < uiDiff)
            {
                uint8 uiMaxWraith = urand(3, 4);
                for (uint8 i = 0; i < uiMaxWraith; ++i)
                    DoCastSpellIfCan(m_creature, aWraithSummonSpells[i], CAST_TRIGGERED);

                DoScriptText(SAY_SUMMON, m_creature);
                m_uiSummonTimer = urand(45000, 50000);
            }
            else
                m_uiSummonTimer -= uiDiff;

            if (m_uiAngerTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DISGRUNTLED_ANGER) == CAST_OK)
                    m_uiAngerTimer = urand(55000, 84000);
            }
            else
                m_uiAngerTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_nether_wraithAI : public ScriptedAI
{
    mob_nether_wraithAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiArcaneMissilesTimer;
    bool m_bHasDetonated;

    void Reset() override
    {
        m_uiArcaneMissilesTimer = urand(1000, 4000);
        m_bHasDetonated         = false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneMissilesTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ARCANE_BOLT) == CAST_OK)
                    m_uiArcaneMissilesTimer = urand(5000, 10000);
            }
        }
        else
            m_uiArcaneMissilesTimer -= uiDiff;

        if (!m_bHasDetonated && m_creature->GetHealthPercent() < 10.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DETONATION, CAST_TRIGGERED) == CAST_OK)
            {
                // Selfkill after the detonation
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                m_bHasDetonated = true;
                return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_pathaleon_the_calculator(Creature* pCreature)
{
    return new boss_pathaleon_the_calculatorAI(pCreature);
}

CreatureAI* GetAI_mob_nether_wraith(Creature* pCreature)
{
    return new mob_nether_wraithAI(pCreature);
}

void AddSC_boss_pathaleon_the_calculator()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_pathaleon_the_calculator";
    pNewScript->GetAI = &GetAI_boss_pathaleon_the_calculator;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nether_wraith";
    pNewScript->GetAI = &GetAI_mob_nether_wraith;
    pNewScript->RegisterSelf();
}
