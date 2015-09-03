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
SDName: boss_erekem
SD%Complete: 90
SDComment: Timers may need adjustments
SDCategory: Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SAY_AGGRO                   = -1608012,
    SAY_ADD_DIE_1               = -1608013,
    SAY_ADD_DIE_2               = -1608014,
    SAY_DEATH                   = -1608018,
    // A few Sound IDs on SLAY, if there _is_ text related, fields -1608015 to -1608017 are free
    SOUND_ID_SLAY_1             = 14222,
    SOUND_ID_SLAY_2             = 14223,
    SOUND_ID_SLAY_3             = 14224,

    SPELL_BLOODLUST             = 54516,
    SPELL_BREAK_BONDS_H         = 59463,
    SPELL_CHAIN_HEAL            = 54481,
    SPELL_CHAIN_HEAL_H          = 59473,
    SPELL_EARTH_SHIELD          = 54479,
    SPELL_EARTH_SHIELD_H        = 59471,
    SPELL_EARTH_SHOCK           = 54511,
    SPELL_LIGHTNING_BOLT        = 53044,
    SPELL_STORMSTRIKE           = 51876,

    // Spells of adds
    SPELL_GUSHING_WOUND         = 39215,
    SPELL_HOWLING_SCREECH       = 54463,
    SPELL_STRIKE                = 14516
};

struct boss_erekemAI : public ScriptedAI
{
    boss_erekemAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_violet_hold*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        Reset();
    }

    instance_violet_hold* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBreakBondsTimer;
    uint32 m_uiChainHealTimer;
    uint32 m_uiEarthShieldTimer;
    uint32 m_uiEarthShockTimer;
    uint32 m_uiSpecialSpellTimer;
    uint8 m_uiGuardiansDead;

    void Reset() override
    {
        m_uiSpecialSpellTimer   = 0;
        m_uiEarthShieldTimer    = urand(2000, 3000);
        m_uiEarthShockTimer     = urand(4000, 9000);
        m_uiChainHealTimer      = urand(5000, 15000);
        m_uiBreakBondsTimer     = urand(25000, 30000);
        m_uiGuardiansDead       = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoPlaySoundToSet(m_creature, SOUND_ID_SLAY_1); break;
            case 1: DoPlaySoundToSet(m_creature, SOUND_ID_SLAY_2); break;
            case 2: DoPlaySoundToSet(m_creature, SOUND_ID_SLAY_3); break;
        }
    }

    void GuardianJustDied()
    {
        DoScriptText(!m_uiGuardiansDead ? SAY_ADD_DIE_1 : SAY_ADD_DIE_2, m_creature);
        ++m_uiGuardiansDead;

        // cast bloodlust if both guards are dead
        if (m_uiGuardiansDead == 2)
            DoCastSpellIfCan(m_creature, SPELL_BLOODLUST, CAST_INTERRUPT_PREVIOUS);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEarthShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_EARTH_SHIELD : SPELL_EARTH_SHIELD_H, CAST_AURA_NOT_PRESENT) == CAST_OK)
                m_uiEarthShieldTimer = urand(25000, 30000);
        }
        else
            m_uiEarthShieldTimer -= uiDiff;

        if (m_uiEarthShockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_EARTH_SHOCK) == CAST_OK)
                    m_uiEarthShockTimer = urand(8000, 13000);
            }
        }
        else
            m_uiEarthShockTimer -= uiDiff;

        if (m_uiChainHealTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CHAIN_HEAL : SPELL_CHAIN_HEAL_H) == CAST_OK)
                m_uiChainHealTimer = urand(15000, 25000);
        }
        else
            m_uiChainHealTimer -= uiDiff;

        // Cast Stormstrike only if both guards are down
        if (m_uiSpecialSpellTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_uiGuardiansDead == 2 ? SPELL_STORMSTRIKE : SPELL_LIGHTNING_BOLT) == CAST_OK)
                m_uiSpecialSpellTimer = urand(2000, 3000);
        }
        else
            m_uiSpecialSpellTimer -= uiDiff;

        // Break bonds only on heroic
        if (!m_bIsRegularMode)
        {
            if (m_uiBreakBondsTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BREAK_BONDS_H) == CAST_OK)
                    m_uiBreakBondsTimer = urand(25000, 30000);
            }
            else
                m_uiBreakBondsTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_erekem(Creature* pCreature)
{
    return new boss_erekemAI(pCreature);
}

struct npc_erekem_guardAI : public ScriptedAI
{
    npc_erekem_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_violet_hold*)pCreature->GetInstanceData());
        Reset();
    }

    instance_violet_hold* m_pInstance;

    uint32 m_uiGushingWoundTimer;
    uint32 m_uiHowlingScreechTimer;
    uint32 m_uiStrikeTimer;

    void Reset() override
    {
        m_uiGushingWoundTimer   = urand(9000, 14000);
        m_uiHowlingScreechTimer = urand(8000, 12000);
        m_uiStrikeTimer         = urand(5000, 7000);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        if (Creature* pBoss = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(TYPE_EREKEM) != DONE ? NPC_EREKEM : NPC_ARAKKOA))
        {
            if (!pBoss->isAlive())
                return;

            ((boss_erekemAI*)pBoss->AI())->GuardianJustDied();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiGushingWoundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GUSHING_WOUND) == CAST_OK)
                m_uiGushingWoundTimer = urand(25000, 30000);
        }
        else
            m_uiGushingWoundTimer -= uiDiff;

        if (m_uiHowlingScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HOWLING_SCREECH) == CAST_OK)
                m_uiHowlingScreechTimer = urand(10000, 16000);
        }
        else
            m_uiHowlingScreechTimer -= uiDiff;

        if (m_uiStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE) == CAST_OK)
                m_uiStrikeTimer = urand(5000, 7000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_erekem_guard(Creature* pCreature)
{
    return new npc_erekem_guardAI(pCreature);
}

void AddSC_boss_erekem()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_erekem";
    pNewScript->GetAI = &GetAI_boss_erekem;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_erekem_guard";
    pNewScript->GetAI = &GetAI_npc_erekem_guard;
    pNewScript->RegisterSelf();
}
