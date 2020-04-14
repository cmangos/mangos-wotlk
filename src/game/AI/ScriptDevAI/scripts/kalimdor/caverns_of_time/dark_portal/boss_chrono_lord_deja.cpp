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
SDName: Boss_Chrono_Lord_Deja
SD%Complete: 90
SDComment: Small adjustments; Timers
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "dark_portal.h"

enum
{
    SAY_AGGRO                   = -1269007,
    SAY_BANISH                  = -1269008,
    SAY_SLAY1                   = -1269009,
    SAY_SLAY2                   = -1269010,
    SAY_DEATH                   = -1269011,

    SPELL_ARCANE_BLAST          = 31457,
    SPELL_ARCANE_BLAST_H        = 38538,
    SPELL_ARCANE_DISCHARGE      = 31472,
    SPELL_ARCANE_DISCHARGE_H    = 38539,
    SPELL_TIME_LAPSE            = 31467,
    SPELL_ATTRACTION            = 38540
};

struct boss_chrono_lord_dejaAI : public ScriptedAI
{
    boss_chrono_lord_dejaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiTimeLapseTimer;
    uint32 m_uiAttractionTimer;
    uint32 m_uiArcaneDischargeTimer;

    void Reset() override
    {
        m_uiArcaneBlastTimer     = urand(18000, 23000);
        m_uiTimeLapseTimer       = urand(10000, 15000);
        m_uiArcaneDischargeTimer = urand(20000, 30000);
        m_uiAttractionTimer      = urand(25000, 35000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Despawn Time Keeper
        if (pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_TIME_KEEPER)
        {
            if (m_creature->IsWithinDistInMap(pWho, 20.0f))
            {
                if (DoCastSpellIfCan(pWho, SPELL_BANISH_HELPER) == CAST_OK)
                    DoScriptText(SAY_BANISH, m_creature);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Arcane Blast
        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_ARCANE_BLAST : SPELL_ARCANE_BLAST_H) == CAST_OK)
                m_uiArcaneBlastTimer = urand(15000, 25000);
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // Arcane Discharge
        if (m_uiArcaneDischargeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_DISCHARGE : SPELL_ARCANE_DISCHARGE_H) == CAST_OK)
                m_uiArcaneDischargeTimer = urand(20000, 30000);
        }
        else
            m_uiArcaneDischargeTimer -= uiDiff;

        // Time Lapse
        if (m_uiTimeLapseTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TIME_LAPSE) == CAST_OK)
                m_uiTimeLapseTimer = urand(15000, 25000);
        }
        else
            m_uiTimeLapseTimer -= uiDiff;

        // Attraction
        if (!m_bIsRegularMode)
        {
            if (m_uiAttractionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ATTRACTION) == CAST_OK)
                    m_uiAttractionTimer = urand(25000, 35000);
            }
            else
                m_uiAttractionTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_chrono_lord_deja(Creature* pCreature)
{
    return new boss_chrono_lord_dejaAI(pCreature);
}

void AddSC_boss_chrono_lord_deja()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_chrono_lord_deja";
    pNewScript->GetAI = &GetAI_boss_chrono_lord_deja;
    pNewScript->RegisterSelf();
}
