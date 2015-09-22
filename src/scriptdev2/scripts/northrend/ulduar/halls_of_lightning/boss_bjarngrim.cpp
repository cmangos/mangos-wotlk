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
SDName: Boss General Bjarngrim
SD%Complete: 90%
SDComment: Waypoint needed, we expect boss to always have 2x Stormforged Lieutenant following
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    // Yell
    SAY_AGGRO                               = -1602000,
    SAY_SLAY_1                              = -1602001,
    SAY_SLAY_2                              = -1602002,
    SAY_SLAY_3                              = -1602003,
    SAY_DEATH                               = -1602004,
    SAY_BATTLE_STANCE                       = -1602005,
    EMOTE_BATTLE_STANCE                     = -1602006,
    SAY_BERSEKER_STANCE                     = -1602007,
    EMOTE_BERSEKER_STANCE                   = -1602008,
    SAY_DEFENSIVE_STANCE                    = -1602009,
    EMOTE_DEFENSIVE_STANCE                  = -1602010,

    SPELL_DEFENSIVE_STANCE                  = 53790,
    SPELL_SPELL_REFLECTION                  = 36096,
    SPELL_PUMMEL                            = 12555,
    SPELL_KNOCK_AWAY                        = 52029,
    SPELL_IRONFORM                          = 52022,

    SPELL_BERSEKER_STANCE                   = 53791,
    SPELL_INTERCEPT                         = 58769,
    SPELL_WHIRLWIND                         = 52027,
    SPELL_CLEAVE                            = 15284,

    SPELL_BATTLE_STANCE                     = 53792,
    SPELL_MORTAL_STRIKE                     = 16856,
    SPELL_SLAM                              = 52026,

    // Other spells - handled by DB wp movement script
    // SPELL_CHARGE_UP                      = 52098,        // only used when starting walk from one platform to the other
    SPELL_TEMPORARY_ELECTRICAL_CHARGE       = 52092,        // triggered part of above

    NPC_STORMFORGED_LIEUTENANT              = 29240,
    SPELL_ARC_WELD                          = 59085,
    SPELL_RENEW_STEEL_N                     = 52774,
    SPELL_RENEW_STEEL_H                     = 59160,

    STANCE_DEFENSIVE                        = 0,
    STANCE_BERSERKER                        = 1,
    STANCE_BATTLE                           = 2
};

/*######
## boss_bjarngrim
######*/

struct boss_bjarngrimAI : public ScriptedAI
{
    boss_bjarngrimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiStance = STANCE_DEFENSIVE;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsChangingStance;

    uint8 m_uiChargingStatus;
    uint8 m_uiStance;

    uint32 m_uiChargeTimer;
    uint32 m_uiChangeStanceTimer;

    uint32 m_uiReflectionTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiPummelTimer;
    uint32 m_uiIronformTimer;

    uint32 m_uiInterceptTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiCleaveTimer;

    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiSlamTimer;

    void Reset() override
    {
        m_bIsChangingStance     = false;

        m_uiChargingStatus      = 0;
        m_uiChargeTimer         = 1000;

        m_uiChangeStanceTimer   = urand(20000, 25000);

        m_uiReflectionTimer     = 8000;
        m_uiKnockAwayTimer      = 20000;
        m_uiPummelTimer         = 10000;
        m_uiIronformTimer       = 25000;

        m_uiInterceptTimer      = 5000;
        m_uiWhirlwindTimer      = 10000;
        m_uiCleaveTimer         = 8000;

        m_uiMortalStrikeTimer   = 8000;
        m_uiSlamTimer           = 10000;

        if (m_uiStance != STANCE_DEFENSIVE)
        {
            DoCastSpellIfCan(m_creature, SPELL_DEFENSIVE_STANCE);
            m_uiStance = STANCE_DEFENSIVE;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
        {
            // Set the achiev in progress
            if (m_creature->HasAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE))
                m_pInstance->SetData(TYPE_BJARNGRIM, SPECIAL);

            m_pInstance->SetData(TYPE_BJARNGRIM, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BJARNGRIM, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BJARNGRIM, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Change stance
        if (m_uiChangeStanceTimer < uiDiff)
        {
            // wait for current spell to finish before change stance
            if (m_creature->IsNonMeleeSpellCasted(false))
                return;

            int uiTempStance = rand() % (3 - 1);

            if (uiTempStance >= m_uiStance)
                ++uiTempStance;

            m_uiStance = uiTempStance;

            switch (m_uiStance)
            {
                case STANCE_DEFENSIVE:
                    DoScriptText(SAY_DEFENSIVE_STANCE, m_creature);
                    DoScriptText(EMOTE_DEFENSIVE_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_DEFENSIVE_STANCE);
                    break;
                case STANCE_BERSERKER:
                    DoScriptText(SAY_BERSEKER_STANCE, m_creature);
                    DoScriptText(EMOTE_BERSEKER_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_BERSEKER_STANCE);
                    break;
                case STANCE_BATTLE:
                    DoScriptText(SAY_BATTLE_STANCE, m_creature);
                    DoScriptText(EMOTE_BATTLE_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_BATTLE_STANCE);
                    break;
            }

            m_uiChangeStanceTimer = urand(20000, 25000);
            return;
        }
        else
            m_uiChangeStanceTimer -= uiDiff;

        switch (m_uiStance)
        {
            case STANCE_DEFENSIVE:
            {
                if (m_uiReflectionTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SPELL_REFLECTION) == CAST_OK)
                        m_uiReflectionTimer = urand(8000, 9000);
                }
                else
                    m_uiReflectionTimer -= uiDiff;

                if (m_uiKnockAwayTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_KNOCK_AWAY) == CAST_OK)
                        m_uiKnockAwayTimer = urand(20000, 21000);
                }
                else
                    m_uiKnockAwayTimer -= uiDiff;

                if (m_uiPummelTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PUMMEL) == CAST_OK)
                        m_uiPummelTimer = urand(10000, 11000);
                }
                else
                    m_uiPummelTimer -= uiDiff;

                if (m_uiIronformTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_IRONFORM) == CAST_OK)
                        m_uiIronformTimer = urand(25000, 26000);
                }
                else
                    m_uiIronformTimer -= uiDiff;

                break;
            }
            case STANCE_BERSERKER:
            {
                if (m_uiInterceptTimer < uiDiff)
                {
                    // not much point is this, better random target and more often?
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTERCEPT) == CAST_OK)
                        m_uiInterceptTimer = urand(45000, 46000);
                }
                else
                    m_uiInterceptTimer -= uiDiff;

                if (m_uiWhirlwindTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                        m_uiWhirlwindTimer = urand(10000, 11000);
                }
                else
                    m_uiWhirlwindTimer -= uiDiff;

                if (m_uiCleaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(8000, 9000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                break;
            }
            case STANCE_BATTLE:
            {
                if (m_uiMortalStrikeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE) == CAST_OK)
                        m_uiMortalStrikeTimer = urand(20000, 21000);
                }
                else
                    m_uiMortalStrikeTimer -= uiDiff;

                if (m_uiSlamTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLAM) == CAST_OK)
                        m_uiSlamTimer = urand(15000, 16000);
                }
                else
                    m_uiSlamTimer -= uiDiff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_stormforged_lieutenant
######*/

struct mob_stormforged_lieutenantAI : public ScriptedAI
{
    mob_stormforged_lieutenantAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcWeldTimer;
    uint32 m_uiRenewSteelTimer;

    void Reset() override
    {
        m_uiArcWeldTimer    = urand(20000, 21000);
        m_uiRenewSteelTimer = urand(10000, 11000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcWeldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARC_WELD) == CAST_OK)
                m_uiArcWeldTimer = urand(20000, 21000);
        }
        else
            m_uiArcWeldTimer -= uiDiff;

        if (m_uiRenewSteelTimer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pBjarngrim = m_pInstance->GetSingleCreatureFromStorage(NPC_BJARNGRIM))
                {
                    if (pBjarngrim->isAlive())
                        DoCastSpellIfCan(pBjarngrim, m_bIsRegularMode ? SPELL_RENEW_STEEL_N : SPELL_RENEW_STEEL_H);
                }
            }
            m_uiRenewSteelTimer = urand(10000, 14000);
        }
        else
            m_uiRenewSteelTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bjarngrim(Creature* pCreature)
{
    return new boss_bjarngrimAI(pCreature);
}

CreatureAI* GetAI_mob_stormforged_lieutenant(Creature* pCreature)
{
    return new mob_stormforged_lieutenantAI(pCreature);
}

void AddSC_boss_bjarngrim()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_bjarngrim";
    pNewScript->GetAI = &GetAI_boss_bjarngrim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_stormforged_lieutenant";
    pNewScript->GetAI = &GetAI_mob_stormforged_lieutenant;
    pNewScript->RegisterSelf();
}
