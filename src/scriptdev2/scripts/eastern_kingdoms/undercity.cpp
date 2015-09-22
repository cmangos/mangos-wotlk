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
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 9180(post-event).
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_lady_sylvanas_windrunner
EndContentData */

#include "precompiled.h"

/*######
## npc_lady_sylvanas_windrunner
######*/

enum
{
    EMOTE_LAMENT_START          = -1000193,
    SAY_LAMENT_END              = -1000196,
    EMOTE_LAMENT_END            = -1000197,

    SPELL_HIGHBORNE_AURA        = 37090,
    SPELL_SYLVANAS_CAST         = 36568,
    SPELL_RIBBON_OF_SOULS       = 37099,

    NPC_HIGHBORNE_LAMENTER      = 21628,
    NPC_HIGHBORNE_BUNNY         = 21641,

    QUEST_ID_JOURNEY_UNDERCITY  = 9180,

    MAX_LAMENTERS               = 4,
};

static const float aHighborneLoc[MAX_LAMENTERS][4] =
{
    {1285.41f, 312.47f, -61.0f, 0.51f},
    {1286.96f, 310.40f, -61.0f, 1.00f},
    {1289.66f, 309.66f, -61.0f, 1.52f},
    {1292.51f, 310.50f, -61.0f, 1.99f},
};

struct npc_lady_sylvanas_windrunnerAI : public ScriptedAI
{
    npc_lady_sylvanas_windrunnerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiLamentEventTimer;
    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_uiLamentEventTimer = 0;
        m_uiSummonTimer = 0;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HIGHBORNE_BUNNY)
            pSummoned->CastSpell(pSummoned, SPELL_RIBBON_OF_SOULS, false);
        else if (pSummoned->GetEntry() == NPC_HIGHBORNE_LAMENTER)
        {
            pSummoned->CastSpell(pSummoned, SPELL_HIGHBORNE_AURA, false);

            pSummoned->SetLevitate(true);
            pSummoned->GetMotionMaster()->MovePoint(0, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() + 5.0f);
        }
    }

    void DoStartLamentEvent()
    {
        DoScriptText(EMOTE_LAMENT_START, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SYLVANAS_CAST);
        m_uiSummonTimer = 13000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiLamentEventTimer)
        {
            if (m_uiLamentEventTimer <= uiDiff)
            {
                float fX, fY, fZ;
                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
                m_creature->SummonCreature(NPC_HIGHBORNE_BUNNY, fX, fY, fZ + 15.0f, 0, TEMPSUMMON_TIMED_DESPAWN, 3000);

                m_uiLamentEventTimer = 2000;

                if (!m_creature->HasAura(SPELL_SYLVANAS_CAST))
                {
                    DoScriptText(SAY_LAMENT_END, m_creature);
                    DoScriptText(EMOTE_LAMENT_END, m_creature);
                    m_uiLamentEventTimer = 0;
                }
            }
            else
                m_uiLamentEventTimer -= uiDiff;
        }

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                for (uint8 i = 0; i < MAX_LAMENTERS; ++i)
                    m_creature->SummonCreature(NPC_HIGHBORNE_LAMENTER, aHighborneLoc[i][0], aHighborneLoc[i][1], aHighborneLoc[i][2], aHighborneLoc[i][3], TEMPSUMMON_TIMED_DESPAWN, 160000);

                m_uiLamentEventTimer = 2000;
                m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lady_sylvanas_windrunner(Creature* pCreature)
{
    return new npc_lady_sylvanas_windrunnerAI(pCreature);
}

bool QuestRewarded_npc_lady_sylvanas_windrunner(Player* /*pPlayer*/, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_JOURNEY_UNDERCITY)
    {
        if (npc_lady_sylvanas_windrunnerAI* pSylvanAI = dynamic_cast<npc_lady_sylvanas_windrunnerAI*>(pCreature->AI()))
            pSylvanAI->DoStartLamentEvent();
    }

    return true;
}

void AddSC_undercity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_sylvanas_windrunner";
    pNewScript->GetAI = &GetAI_npc_lady_sylvanas_windrunner;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_lady_sylvanas_windrunner;
    pNewScript->RegisterSelf();
}
