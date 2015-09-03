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
SDName: Boss_NexusPrince_Shaffar
SD%Complete: 100
SDComment: ToDo: move the Ethereal Beacon script to eventAI
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

/* ContentData
boss_nexusprince_shaffar
mob_ethereal_beacon
EndContentData */

#include "precompiled.h"

enum
{
    SAY_INTRO                       = -1557000,
    SAY_AGGRO_1                     = -1557001,
    SAY_AGGRO_2                     = -1557002,
    SAY_AGGRO_3                     = -1557003,
    SAY_SLAY_1                      = -1557004,
    SAY_SLAY_2                      = -1557005,
    SAY_SUMMON                      = -1557006,
    SAY_DEAD                        = -1557007,

    SPELL_BLINK                     = 34605,
    SPELL_FROSTBOLT                 = 32364,
    SPELL_FIREBALL                  = 32363,
    SPELL_FROSTNOVA                 = 32365,

    SPELL_ETHEREAL_BEACON           = 32371,                // Summons 18431
    // SPELL_ETHEREAL_BEACON_VISUAL  = 32368,               // included in creature_template_addon
};

struct boss_nexusprince_shaffarAI : public ScriptedAI
{
    boss_nexusprince_shaffarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bHasTaunted = false;
        Reset();
    }

    uint32 m_uiBlinkTimer;
    uint32 m_uiBeaconTimer;
    uint32 m_uiFireBallTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiFrostNovaTimer;

    bool m_bHasTaunted;

    void Reset() override
    {
        m_uiBlinkTimer      = 30000;
        m_uiBeaconTimer     = urand(12000, 15000);
        m_uiFireBallTimer   = urand(2000, 12000);
        m_uiFrostboltTimer  = urand(1000, 14000);
        m_uiFrostNovaTimer  = urand(18000, 25000);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 100.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEAD, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFrostNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FROSTNOVA) == CAST_OK)
                m_uiFrostNovaTimer = urand(10000, 20000);
        }
        else
            m_uiFrostNovaTimer -= uiDiff;

        if (m_uiFrostboltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT) == CAST_OK)
                m_uiFrostboltTimer = urand(3000, 8000);
        }
        else
            m_uiFrostboltTimer -= uiDiff;

        if (m_uiFireBallTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireBallTimer = urand(3000, 8000);
        }
        else
            m_uiFireBallTimer -= uiDiff;

        if (m_uiBlinkTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLINK) == CAST_OK)
            {
                // expire movement, will prevent from running right back to victim after cast
                //(but should MoveChase be used again at a certain time or should he not move?)
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                    m_creature->GetMotionMaster()->MovementExpired();

                m_uiBlinkTimer = urand(25000, 30000);
            }
        }
        else
            m_uiBlinkTimer -= uiDiff;

        if (m_uiBeaconTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ETHEREAL_BEACON) == CAST_OK)
            {
                if (!urand(0, 3))
                    DoScriptText(SAY_SUMMON, m_creature);

                m_uiBeaconTimer = urand(45000, 75000);
            }
        }
        else
            m_uiBeaconTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nexusprince_shaffar(Creature* pCreature)
{
    return new boss_nexusprince_shaffarAI(pCreature);
}

void AddSC_boss_nexusprince_shaffar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_nexusprince_shaffar";
    pNewScript->GetAI = &GetAI_boss_nexusprince_shaffar;
    pNewScript->RegisterSelf();
}
