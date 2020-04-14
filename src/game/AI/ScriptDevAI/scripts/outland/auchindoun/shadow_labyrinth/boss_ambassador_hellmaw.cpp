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
SDName: Boss_Ambassador_Hellmaw
SD%Complete: 80
SDComment: Enrage spell missing/not known
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shadow_labyrinth.h"

enum
{
    SAY_AGGRO_1             = -1555001,
    SAY_AGGRO_2             = -1555002,
    SAY_AGGRO_3             = -1555003,
    SAY_HELP                = -1555004,
    SAY_SLAY_1              = -1555005,
    SAY_SLAY_2              = -1555006,
    SAY_DEATH               = -1555007,

    SPELL_CORROSIVE_ACID    = 33551,
    SPELL_FEAR              = 33547,
    SPELL_ENRAGE            = 34970
};

struct boss_ambassador_hellmawAI : public ScriptedAI
{
    boss_ambassador_hellmawAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_shadow_labyrinth*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetCanEnterCombat(false);
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    instance_shadow_labyrinth* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBanishTimer;
    uint32 m_uiCorrosiveAcidTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiEnrageTimer;
    bool m_bIsEnraged;

    void Reset() override
    {
        m_uiBanishTimer         = 2000;
        m_uiCorrosiveAcidTimer  = urand(20000, 23000);
        m_uiFearTimer           = urand(20000, 26000);
        m_uiEnrageTimer         = 3 * MINUTE * IN_MILLISECONDS;
        m_bIsEnraged            = false;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // yell intro and remove banish aura
            DoScriptText(SAY_HELLMAW_INTRO, m_creature);
            m_creature->GetMotionMaster()->MoveWaypoint();
            m_creature->RemoveAurasDueToSpell(SPELL_BANISH);
            m_creature->SetCanEnterCombat(true);
            SetReactState(REACT_AGGRESSIVE);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HELLMAW, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HELLMAW, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HELLMAW, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBanishTimer)
        {
            if (m_uiBanishTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                // Check for banish
                if (m_pInstance->IsHellmawUnbanished())
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_BANISH);
                    SetReactState(REACT_AGGRESSIVE);
                    m_creature->GetMotionMaster()->MoveWaypoint();
                    m_uiBanishTimer = 0;
                }
            }
            else
                m_uiBanishTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiCorrosiveAcidTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CORROSIVE_ACID) == CAST_OK)
                m_uiCorrosiveAcidTimer = urand(23000, 35000);
        }
        else
            m_uiCorrosiveAcidTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FEAR) == CAST_OK)
                m_uiFearTimer = urand(20000, 38000);
        }
        else
            m_uiFearTimer -= uiDiff;

        if (!m_bIsRegularMode && !m_bIsEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    m_bIsEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_ambassador_hellmaw(Creature* pCreature)
{
    return new boss_ambassador_hellmawAI(pCreature);
}

void AddSC_boss_ambassador_hellmaw()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ambassador_hellmaw";
    pNewScript->GetAI = &GetAI_boss_ambassador_hellmaw;
    pNewScript->RegisterSelf();
}
