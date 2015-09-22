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
SDName: Boss_Bloodboil
SD%Complete: 90
SDComment: Timers may need adjustments.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    // Speech'n'Sound
    SAY_AGGRO                   = -1564029,
    SAY_SLAY1                   = -1564030,
    SAY_SLAY2                   = -1564031,
    SAY_SPECIAL1                = -1564032,
    SAY_SPECIAL2                = -1564033,
    SAY_ENRAGE1                 = -1564034,
    SAY_ENRAGE2                 = -1564035,
    SAY_DEATH                   = -1564036,

    // Spells
    // Phase 1
    SPELL_FEL_ACID_1            = 40508,
    SPELL_ARCING_SMASH_1        = 40457,
    SPELL_EJECT_1               = 40486,
    SPELL_ACIDIC_WOUND          = 40481,
    SPELL_BLOODBOIL             = 42005,
    SPELL_BEWILDERING_STRIKE    = 40491,

    // Phase 2
    SPELL_ACID_GEYSER           = 40630,
    SPELL_FEL_ACID_2            = 40595,
    SPELL_ARCING_SMASH_2        = 40599,
    SPELL_EJECT_2               = 40597,
    SPELL_INSIGNIFIGANCE        = 40618,
    SPELL_FEL_RAGE              = 40594,
    SPELL_FEL_RAGE_PLAYER_1     = 40604,
    SPELL_FEL_RAGE_PLAYER_2     = 40616,
    SPELL_FEL_RAGE_PLAYER_3     = 41625,
    SPELL_FEL_RAGE_4            = 40617,                // spell not confirmed
    SPELL_FEL_RAGE_5            = 46787,                // spell not confirmed
    SPELL_TAUNT_GURTOGG         = 40603,

    // Other spells
    SPELL_CHARGE                = 40602,                // spell not confirmed
    SPELL_BERSERK               = 27680,

    MAX_BLOODBOILS              = 5,
};

struct boss_gurtogg_bloodboilAI : public ScriptedAI
{
    boss_gurtogg_bloodboilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBloodboilTimer;
    uint32 m_uiAcidGeyserTimer;
    uint32 m_uiAcidicWoundTimer;
    uint32 m_uiArcingSmashTimer;
    uint32 m_uiFelAcidTimer;
    uint32 m_uiEjectTimer;
    uint32 m_uiStrikeTimer;
    uint32 m_uiPhaseChangeTimer;
    uint32 m_uiBerserkTimer;
    uint8 m_uiBloodboilCount;

    bool m_bIsPhase1;

    void Reset() override
    {
        m_uiBloodboilTimer      = 10000;
        m_uiBloodboilCount      = 0;
        m_uiAcidGeyserTimer     = 1000;
        m_uiAcidicWoundTimer    = 6000;
        m_uiArcingSmashTimer    = 19000;
        m_uiFelAcidTimer        = 25000;
        m_uiEjectTimer          = 10000;
        m_uiStrikeTimer         = 15000;
        m_uiPhaseChangeTimer    = MINUTE * IN_MILLISECONDS;
        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;

        m_bIsPhase1             = true;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLOODBOIL, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLOODBOIL, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLOODBOIL, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcingSmashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsPhase1 ? SPELL_ARCING_SMASH_1 : SPELL_ARCING_SMASH_2) == CAST_OK)
                m_uiArcingSmashTimer = 10000;
        }
        else
            m_uiArcingSmashTimer -= uiDiff;

        if (m_uiFelAcidTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsPhase1 ? SPELL_FEL_ACID_1 : SPELL_FEL_ACID_2) == CAST_OK)
                m_uiFelAcidTimer = 25000;
        }
        else
            m_uiFelAcidTimer -= uiDiff;

        // Phase 1 spells
        if (m_bIsPhase1)
        {
            if (m_uiStrikeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BEWILDERING_STRIKE) == CAST_OK)
                    m_uiStrikeTimer = 20000;
            }
            else
                m_uiStrikeTimer -= uiDiff;

            if (m_uiEjectTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_EJECT_1) == CAST_OK)
                {
                    // Script effect: reduce threat on main target
                    m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -40);
                    m_uiEjectTimer = 15000;
                }
            }
            else
                m_uiEjectTimer -= uiDiff;

            if (m_uiAcidicWoundTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ACIDIC_WOUND) == CAST_OK)
                    m_uiAcidicWoundTimer = 10000;
            }
            else
                m_uiAcidicWoundTimer -= uiDiff;

            if (m_uiBloodboilTimer)
            {
                if (m_uiBloodboilTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BLOODBOIL) == CAST_OK)
                    {
                        ++m_uiBloodboilCount;

                        // Allow only 5 Bloodboils per phase.
                        if (m_uiBloodboilCount == MAX_BLOODBOILS)
                            m_uiBloodboilTimer = 0;
                        else
                            m_uiBloodboilTimer = 10000;
                    }
                }
                else
                    m_uiBloodboilTimer -= uiDiff;
            }
        }
        // Phase 2 spells
        else
        {
            if (m_uiAcidGeyserTimer)
            {
                if (m_uiAcidGeyserTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ACID_GEYSER) == CAST_OK)
                        m_uiAcidGeyserTimer = 0;
                }
                else
                    m_uiAcidGeyserTimer -= uiDiff;
            }

            if (m_uiEjectTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_EJECT_2) == CAST_OK)
                    m_uiEjectTimer = 15000;
            }
            else
                m_uiEjectTimer -= uiDiff;
        }

        if (m_uiPhaseChangeTimer < uiDiff)
        {
            if (m_bIsPhase1)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    // Buff self
                    if (DoCastSpellIfCan(m_creature, SPELL_FEL_RAGE) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, m_creature);

                        // Debuff player
                        DoCastSpellIfCan(pTarget, SPELL_FEL_RAGE_PLAYER_1, CAST_TRIGGERED);
                        DoCastSpellIfCan(pTarget, SPELL_FEL_RAGE_PLAYER_2, CAST_TRIGGERED);
                        DoCastSpellIfCan(pTarget, SPELL_FEL_RAGE_PLAYER_3, CAST_TRIGGERED);
                        // Allow player to taunt Gurtogg
                        pTarget->CastSpell(m_creature, SPELL_TAUNT_GURTOGG, true);

                        // Don't allow others to generate threat
                        DoCastSpellIfCan(m_creature, SPELL_INSIGNIFIGANCE, CAST_TRIGGERED);

                        // Reset timers
                        m_bIsPhase1          = false;
                        m_uiAcidGeyserTimer  = 1000;
                        m_uiPhaseChangeTimer = 30000;
                    }
                }
            }
            else
            {
                // Reset timers
                m_bIsPhase1          = true;
                m_uiBloodboilTimer   = 10000;
                m_uiBloodboilCount   = 0;
                m_uiAcidicWoundTimer += 2000;
                m_uiArcingSmashTimer += 2000;
                m_uiFelAcidTimer     += 2000;
                m_uiEjectTimer       += 2000;
                m_uiPhaseChangeTimer = 60000;
            }
        }
        else
            m_uiPhaseChangeTimer -= uiDiff;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_ENRAGE1 : SAY_ENRAGE2, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gurtogg_bloodboil(Creature* pCreature)
{
    return new boss_gurtogg_bloodboilAI(pCreature);
}

void AddSC_boss_gurtogg_bloodboil()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gurtogg_bloodboil";
    pNewScript->GetAI = &GetAI_boss_gurtogg_bloodboil;
    pNewScript->RegisterSelf();
}
