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
SD%Complete: 100%
SDComment:
SDCategory: Azjol'Nerub
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
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

enum KrikthirActions
{
    KRIKTHIR_ACTION_FRENZY,
    KRIKTHIR_ACTION_CURSE_FATIGUE,
    KRIKTHIR_ACTION_MINDFLAY,
    KIRKTHIR_ACTION_SWARM,
    KRIKTHIR_ACTION_MAX,
};

/*######
## boss_krikthir
######*/

struct boss_krikthirAI : public CombatAI
{
    boss_krikthirAI(Creature* creature) : CombatAI(creature, KRIKTHIR_ACTION_MAX), m_instance(static_cast<instance_azjol_nerub*>(creature->GetInstanceData()))
    {
        AddCombatAction(KRIKTHIR_ACTION_CURSE_FATIGUE, 30000u);
        AddCombatAction(KRIKTHIR_ACTION_MINDFLAY, 13000u);
        AddCombatAction(KIRKTHIR_ACTION_SWARM, 10000u);
        AddTimerlessCombatAction(KRIKTHIR_ACTION_FRENZY, true);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        m_introSpeech = false;
    }

    instance_azjol_nerub* m_instance;
    bool m_isRegularMode;

    bool m_introSpeech;

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_introSpeech && pWho->IsPlayer() && m_creature->IsWithinDistInMap(pWho, DEFAULT_VISIBILITY_INSTANCE) && m_creature->IsWithinLOSInMap(pWho))
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_PREFIGHT_1, m_creature); break;
                case 1: DoScriptText(SAY_PREFIGHT_2, m_creature); break;
                case 2: DoScriptText(SAY_PREFIGHT_3, m_creature); break;
            }
            m_introSpeech = true;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_KRIKTHIR, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SKITTERING_SWARMER || pSummoned->GetEntry() == NPC_SKITTERING_INFECTOR)
            pSummoned->AI()->AttackStart(m_creature->GetVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KRIKTHIR_ACTION_FRENZY:
                if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                {
                    DoScriptText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case KRIKTHIR_ACTION_CURSE_FATIGUE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_CURSE_OF_FATIGUE : SPELL_CURSE_OF_FATIGUE_H) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case KRIKTHIR_ACTION_MINDFLAY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_MINDFLAY : SPELL_MINDFLAY_H) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case KIRKTHIR_ACTION_SWARM:
                if (DoCastSpellIfCan(m_creature, SPELL_SWARM) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SWARM_1 : SAY_SWARM_2, m_creature);
                    ResetCombatAction(action, 30000);
                }
                break;
        }
    }
};

void AddSC_boss_krikthir()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_krikthir";
    pNewScript->GetAI = &GetNewAIInstance<boss_krikthirAI>;
    pNewScript->RegisterSelf();
}
