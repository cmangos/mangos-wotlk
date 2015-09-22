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
SDName: Boss_Warp_Splinter
SD%Complete: 90
SDComment: Timers may need adjustments
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "precompiled.h"

/*#####
# boss_warp_splinter
#####*/

enum
{
    SAY_AGGRO                   = -1553007,
    SAY_SLAY_1                  = -1553008,
    SAY_SLAY_2                  = -1553009,
    SAY_SUMMON_1                = -1553010,
    SAY_SUMMON_2                = -1553011,
    SAY_DEATH                   = -1553012,

    SPELL_WAR_STOMP             = 34716,
    SPELL_SUMMON_SAPLINGS       = 34741,            // this will leech the health from all saplings
    SPELL_ARCANE_VOLLEY         = 36705,
    SPELL_ARCANE_VOLLEY_H       = 39133,

    NPC_SAPLING                 = 19949,
};

// Summon Saplings spells (too many to declare them above)
static const uint32 aSaplingsSummonSpells[10] = {34727, 34730, 34731, 34732, 34733, 34734, 34735, 34736, 34737, 34739};

struct boss_warp_splinterAI : public ScriptedAI
{
    boss_warp_splinterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        // Add the summon spells to a vector for better handling
        for (uint8 i = 0; i < 10; ++i)
            m_vSummonSpells.push_back(aSaplingsSummonSpells[i]);

        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiWarStompTimer;
    uint32 m_uiSummonTreantsTimer;
    uint32 m_uiArcaneVolleyTimer;

    std::vector<uint32> m_vSummonSpells;

    void Reset() override
    {
        m_uiWarStompTimer       = urand(6000, 7000);
        m_uiSummonTreantsTimer  = urand(25000, 35000);
        m_uiArcaneVolleyTimer   = urand(12000, 14500);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SAPLING)
            pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
    }

    // Wrapper to summon all Saplings
    void SummonTreants()
    {
        // Choose 6 random spells out of 10
        std::random_shuffle(m_vSummonSpells.begin(), m_vSummonSpells.end());
        for (uint8 i = 0; i < 6; ++i)
            DoCastSpellIfCan(m_creature, m_vSummonSpells[i], CAST_TRIGGERED);

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SAPLINGS, CAST_TRIGGERED);
        DoScriptText(urand(0, 1) ? SAY_SUMMON_1 : SAY_SUMMON_2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // War Stomp
        if (m_uiWarStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WAR_STOMP) == CAST_OK)
                m_uiWarStompTimer = urand(17000, 38000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        // Arcane Volley
        if (m_uiArcaneVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                m_uiArcaneVolleyTimer = urand(16000, 38000);
        }
        else
            m_uiArcaneVolleyTimer -= uiDiff;

        // Summon Treants
        if (m_uiSummonTreantsTimer < uiDiff)
        {
            SummonTreants();
            m_uiSummonTreantsTimer = urand(37000, 55000);
        }
        else
            m_uiSummonTreantsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*#####
# mob_treant (Sapling)
#####*/
struct npc_saplingAI  : public ScriptedAI
{
    npc_saplingAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        // ToDo: This one may need further reserch
        // m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_warp_splinter(Creature* pCreature)
{
    return new boss_warp_splinterAI(pCreature);
}

CreatureAI* GetAI_npc_sapling(Creature* pCreature)
{
    return new npc_saplingAI(pCreature);
}

void AddSC_boss_warp_splinter()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_warp_splinter";
    pNewScript->GetAI = &GetAI_boss_warp_splinter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_warp_splinter_treant";
    pNewScript->GetAI = &GetAI_npc_sapling;
    pNewScript->RegisterSelf();
}
