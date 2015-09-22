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
SDName: Boss_Mother_Shahraz
SD%Complete: 80
SDComment: Saber Lash and Fatal Attraction need core support. Timers may need some tunning.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    // Speech'n'Sounds
    SAY_TAUNT_1                 = -1564018,
    SAY_TAUNT_2                 = -1564019,
    SAY_TAUNT_3                 = -1564020,
    SAY_AGGRO                   = -1564021,
    SAY_SPELL_1                 = -1564022,
    SAY_SPELL_2                 = -1564023,
    SAY_SPELL_3                 = -1564024,
    SAY_SLAY_1                  = -1564025,
    SAY_SLAY_2                  = -1564026,
    SAY_ENRAGE                  = -1564027,
    SAY_DEATH                   = -1564028,

    // Spells
    SPELL_SINFUL_PERIODIC       = 40862,        // periodic triggers 40827
    SPELL_SINISTER_PERIODIC     = 40863,        // periodic triggers 40859
    SPELL_VILE_PERIODIC         = 40865,        // periodic triggers 40860
    SPELL_WICKED_PERIODIC       = 40866,        // periodic triggers 40861
    SPELL_FATAL_ATTRACTION      = 40869,        // dummy, triggers 41001
    SPELL_SILENCING_SHRIEK      = 40823,
    SPELL_SABER_LASH_PROC       = 40816,        // procs 40810 and 43690 on melee damage
    SPELL_FRENZY                = 23537,
    SPELL_BERSERK               = 45078,
};

static const uint32 aPrismaticAuras[] =
{
    40880,                                                  // Shadow
    40882,                                                  // Fire
    40883,                                                  // Nature
    40891,                                                  // Arcane
    40896,                                                  // Frost
    40897,                                                  // Holy
};

static const uint32 aPeriodicBeams[] = {SPELL_SINFUL_PERIODIC, SPELL_SINISTER_PERIODIC, SPELL_VILE_PERIODIC, SPELL_WICKED_PERIODIC};

struct boss_shahrazAI : public ScriptedAI
{
    boss_shahrazAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_black_temple*)pCreature->GetInstanceData();
        Reset();
    }

    instance_black_temple* m_pInstance;

    uint32 m_uiBeamTimer;
    uint32 m_uiPrismaticShieldTimer;
    uint32 m_uiFatalAttractionTimer;
    uint32 m_uiShriekTimer;
    uint32 m_uiRandomYellTimer;
    uint32 m_uiBerserkTimer;
    uint8 m_uiCurrentBeam;

    bool m_bIsEnraged;

    void Reset() override
    {
        m_uiBeamTimer               = urand(5000, 10000);
        m_uiCurrentBeam             = urand(0, 3);
        m_uiPrismaticShieldTimer    = 0;
        m_uiFatalAttractionTimer    = 25000;
        m_uiShriekTimer             = 30000;
        m_uiRandomYellTimer         = urand(70000, 110000);
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;

        m_bIsEnraged                = false;

        DoCastSpellIfCan(m_creature, SPELL_SABER_LASH_PROC);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHAHRAZ, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHAHRAZ, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHAHRAZ, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 10.0f && !m_bIsEnraged)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(SAY_ENRAGE, m_creature);
                m_bIsEnraged = true;
            }
        }

        // Randomly cast one beam.
        if (m_uiBeamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, aPeriodicBeams[m_uiCurrentBeam]) == CAST_OK)
            {
                uint8 uiNextBeam = (m_uiCurrentBeam + urand(1, 3)) % 4;
                m_uiCurrentBeam = uiNextBeam;
                m_uiBeamTimer = urand(10000, 13000);
            }
        }
        else
            m_uiBeamTimer -= uiDiff;

        // Random Prismatic Shield every 15 seconds.
        if (m_uiPrismaticShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, aPrismaticAuras[urand(0, 5)]) == CAST_OK)
                m_uiPrismaticShieldTimer = 15000;
        }
        else
            m_uiPrismaticShieldTimer -= uiDiff;

        if (m_uiFatalAttractionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FATAL_ATTRACTION) == CAST_OK)
            {
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_SPELL_1, m_creature); break;
                    case 1: DoScriptText(SAY_SPELL_2, m_creature); break;
                    case 2: DoScriptText(SAY_SPELL_3, m_creature); break;
                }
                m_uiFatalAttractionTimer = urand(30000, 40000);
            }
        }
        else
            m_uiFatalAttractionTimer -= uiDiff;

        if (m_uiShriekTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SILENCING_SHRIEK) == CAST_OK)
                m_uiShriekTimer = 30000;
        }
        else
            m_uiShriekTimer -= uiDiff;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_ENRAGE, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // Random taunts
        if (m_uiRandomYellTimer < uiDiff)
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_TAUNT_1, m_creature); break;
                case 1: DoScriptText(SAY_TAUNT_2, m_creature); break;
                case 2: DoScriptText(SAY_TAUNT_3, m_creature); break;
            }

            m_uiRandomYellTimer = urand(60000, 150000);
        }
        else
            m_uiRandomYellTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shahraz(Creature* pCreature)
{
    return new boss_shahrazAI(pCreature);
}

void AddSC_boss_mother_shahraz()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mother_shahraz";
    pNewScript->GetAI = &GetAI_boss_shahraz;
    pNewScript->RegisterSelf();
}
