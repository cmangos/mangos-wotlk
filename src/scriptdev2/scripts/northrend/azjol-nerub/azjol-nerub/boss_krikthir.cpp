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
SDName: Boss_Krikthir
SD%Complete: 90%
SDComment: Implement Achievement
SDCategory: Azjol'Nerub
EndScriptData */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
    SAY_AGGRO                 = -1601000,
    SAY_KILL_1                = -1601001,
    SAY_KILL_2                = -1601002,
    SAY_KILL_3                = -1601003,
    SAY_PREFIGHT_1            = -1601007,
    SAY_PREFIGHT_2            = -1601008,
    SAY_PREFIGHT_3            = -1601009,
    SAY_SWARM_1               = -1601010,
    SAY_SWARM_2               = -1601011,
    SAY_DEATH                 = -1601012,
    EMOTE_BOSS_GENERIC_FRENZY = -1000005,

    SPELL_SWARM               = 52440,
    SPELL_CURSE_OF_FATIGUE    = 52592,
    SPELL_CURSE_OF_FATIGUE_H  = 59368,
    SPELL_MINDFLAY            = 52586,
    SPELL_MINDFLAY_H          = 59367,
    SPELL_FRENZY              = 28747,

    NPC_SKITTERING_SWARMER    = 28735,
    NPC_SKITTERING_INFECTOR   = 28736
};

/*######
## boss_krikthir
######*/

struct boss_krikthirAI : public ScriptedAI
{
    boss_krikthirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_azjol_nerub*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_azjol_nerub* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bFrenzy;
    bool m_bIntroSpeech;

    uint32 m_uiSwarmTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiMindFlayTimer;

    void Reset() override
    {
        m_uiSwarmTimer    = 15000;
        m_uiCurseTimer    = 20000;
        m_uiMindFlayTimer = 8000;

        m_bIntroSpeech    = false;
        m_bFrenzy         = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bIntroSpeech && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, DEFAULT_VISIBILITY_INSTANCE) && m_creature->IsWithinLOSInMap(pWho))
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_PREFIGHT_1, m_creature); break;
                case 1: DoScriptText(SAY_PREFIGHT_2, m_creature); break;
                case 2: DoScriptText(SAY_PREFIGHT_3, m_creature); break;
            }
            m_bIntroSpeech = true;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRIKTHIR, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        uint32 uiEntry = pSummoned->GetEntry();
        if (uiEntry == NPC_SKITTERING_SWARMER || uiEntry == NPC_SKITTERING_INFECTOR)
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bFrenzy && m_creature->GetHealthPercent() <= 10.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                m_bFrenzy = true;
            }
        }

        if (m_uiCurseTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CURSE_OF_FATIGUE : SPELL_CURSE_OF_FATIGUE_H) == CAST_OK)
                m_uiCurseTimer = 20000;
        }
        else
            m_uiCurseTimer -= uiDiff;

        if (m_uiMindFlayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MINDFLAY : SPELL_MINDFLAY_H) == CAST_OK)
                m_uiMindFlayTimer = 8000;
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        if (m_uiSwarmTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SWARM) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_SWARM_1 : SAY_SWARM_2, m_creature);
                m_uiSwarmTimer = 15000;
            }
        }
        else
            m_uiSwarmTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_krikthir(Creature* pCreature)
{
    return new boss_krikthirAI(pCreature);
}

void AddSC_boss_krikthir()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_krikthir";
    pNewScript->GetAI = &GetAI_boss_krikthir;
    pNewScript->RegisterSelf();
}
