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
SDName: boss_zarithrian
SD%Complete: 100
SDComment:
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum
{
    SAY_AGGRO           = -1724019,
    SAY_SLAY_1          = -1724020,
    SAY_SLAY_2          = -1724021,
    SAY_DEATH           = -1724022,
    SAY_SUMMON          = -1724023,

    SPELL_SUMMON_FLAMECALLER    = 74398,
    SPELL_CLEAVE_ARMOR          = 74367,
    SPELL_INTIMIDATING_ROAR     = 74384,

    NPC_ONYX_FLAMECALLER        = 39814,                // handled in ACID
};

struct boss_zarithrianAI : public ScriptedAI
{
    boss_zarithrianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruby_sanctum*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint32 m_uiCleaveArmorTimer;
    uint32 m_uiIntimidatingRoarTimer;
    uint32 m_uiCallFlamecallerTimer;

    void Reset() override
    {
        m_uiCleaveArmorTimer        = 15000;
        m_uiIntimidatingRoarTimer   = 14000;
        m_uiCallFlamecallerTimer    = 15000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZARITHRIAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZARITHRIAN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZARITHRIAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ONYX_FLAMECALLER)
            pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCleaveArmorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE_ARMOR) == CAST_OK)
                m_uiCleaveArmorTimer = 15000;
        }
        else
            m_uiCleaveArmorTimer -= uiDiff;

        if (m_uiIntimidatingRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INTIMIDATING_ROAR) == CAST_OK)
                m_uiIntimidatingRoarTimer = 32000;
        }
        else
            m_uiIntimidatingRoarTimer -= uiDiff;

        if (m_uiCallFlamecallerTimer < uiDiff)
        {
            if (!m_pInstance)
            {
                script_error_log("Instance Ruby Sanctum: ERROR Failed to load instance data for this instace.");
                return;
            }

            GuidList m_lStalkersGuidList;
            m_pInstance->GetSpawnStalkersGuidList(m_lStalkersGuidList);

            for (GuidList::const_iterator itr = m_lStalkersGuidList.begin(); itr != m_lStalkersGuidList.end(); ++itr)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(*itr))
                    pStalker->CastSpell(pStalker, SPELL_SUMMON_FLAMECALLER, true, NULL, NULL, m_creature->GetObjectGuid());
            }

            DoScriptText(SAY_SUMMON, m_creature);
            m_uiCallFlamecallerTimer = 45000;
        }
        else
            m_uiCallFlamecallerTimer -= uiDiff;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_zarithrian(Creature* pCreature)
{
    return new boss_zarithrianAI(pCreature);
}

void AddSC_boss_zarithrian()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_zarithrian";
    pNewScript->GetAI = &GetAI_boss_zarithrian;
    pNewScript->RegisterSelf();
}
