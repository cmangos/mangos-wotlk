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
SDName: boss_coren_direbrew
SD%Complete: 75
SDComment: Some parts are not complete - requires additional research. Brewmaidens scripts handled in eventAI.
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                       = -1230034,

    // spells
    SPELL_DIREBREW_DISARM           = 47310,
    SPELL_SUMMON_DIREBREW_MINION    = 47375,
    SPELL_DIREBREW_CHARGE           = 47718,
    SPELL_SUMMON_MOLE_MACHINE       = 47691,            // triggers 47690

    // summoned auras
    SPELL_PORT_TO_COREN             = 52850,

    // other summoned spells - currently not used in script
    // SPELL_CHUCK_MUG               = 50276,
    // SPELL_BARRELED_AURA           = 50278,            // used by Ursula
    // SPELL_HAS_BREW                = 47331,            // triggers 47344 - aura which asks for the second brew on item expire
    // SPELL_SEND_FIRST_MUG          = 47333,            // triggers 47345
    // SPELL_SEND_SECOND_MUG         = 47339,            // triggers 47340 - spell triggered by 47344
    // SPELL_BREWMAIDEN_DESPAWN_AURA = 48186,            // purpose unk

    // npcs
    NPC_DIREBREW_MINION             = 26776,
    NPC_ILSA_DIREBREW               = 26764,
    NPC_URSULA_DIREBREW             = 26822,

    // other
    FACTION_HOSTILE                 = 736,

    QUEST_INSULT_COREN              = 12062,

    MAX_DIREBREW_MINIONS            = 3,
};

struct boss_coren_direbrewAI : public ScriptedAI
{
    boss_coren_direbrewAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDisarmTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiSummonTimer;
    uint8 m_uiPhase;

    void Reset() override
    {
        m_uiDisarmTimer     = 10000;
        m_uiChargeTimer     = 5000;
        m_uiSummonTimer     = 15000;
        m_uiPhase           = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // Spawn 3 minions on aggro
        for (uint8 i = 0; i < MAX_DIREBREW_MINIONS; ++i)
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_DIREBREW_MINION, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ILSA_DIREBREW:
            case NPC_URSULA_DIREBREW:
                pSummoned->CastSpell(m_creature, SPELL_PORT_TO_COREN, true);
                break;
        }

        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Spawn Ilsa
        if (m_creature->GetHealthPercent() < 66.0f && m_uiPhase == 0)
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10, fX, fY, fZ);
            m_creature->SummonCreature(NPC_ILSA_DIREBREW, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            m_uiPhase = 1;
        }

        // Spawn Ursula
        if (m_creature->GetHealthPercent() < 33.0f && m_uiPhase == 1)
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10, fX, fY, fZ);
            m_creature->SummonCreature(NPC_URSULA_DIREBREW, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            m_uiPhase = 2;
        }

        if (m_uiDisarmTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DIREBREW_DISARM) == CAST_OK)
                m_uiDisarmTimer = 15000;
        }
        else
            m_uiDisarmTimer -= uiDiff;

        if (m_uiChargeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DIREBREW_CHARGE, SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DIREBREW_CHARGE) == CAST_OK)
                    m_uiChargeTimer = urand(5000, 10000);
            }
        }
        else
            m_uiChargeTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            for (uint8 i = 0; i < MAX_DIREBREW_MINIONS; ++i)
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_DIREBREW_MINION, CAST_TRIGGERED);

            m_uiSummonTimer = 15000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_coren_direbrew(Creature* pCreature)
{
    return new boss_coren_direbrewAI(pCreature);
}

bool QuestRewarded_npc_coren_direbrew(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_INSULT_COREN)
    {
        DoScriptText(SAY_AGGRO, pCreature, pPlayer);

        pCreature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN);
        pCreature->AI()->AttackStart(pPlayer);
    }

    return true;
}

void AddSC_boss_coren_direbrew()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_coren_direbrew";
    pNewScript->GetAI = &GetAI_boss_coren_direbrew;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_coren_direbrew;
    pNewScript->RegisterSelf();
}
