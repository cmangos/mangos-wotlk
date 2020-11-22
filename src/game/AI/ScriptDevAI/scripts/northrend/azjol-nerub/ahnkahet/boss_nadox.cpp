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
SDName: Boss_Nadox
SD%Complete: 90%
SDComment: Some adjustment may be required
SDCategory: Ahn'kahet
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ahnkahet.h"

enum
{
    SAY_AGGRO                     = -1619000,
    SAY_SUMMON_EGG_1              = -1619001,
    SAY_SUMMON_EGG_2              = -1619002,
    SAY_SLAY_1                    = -1619003,
    SAY_SLAY_2                    = -1619004,
    SAY_SLAY_3                    = -1619005,
    SAY_DEATH                     = -1619006,
    EMOTE_HATCH                   = -1619007,

    SPELL_BROOD_PLAGUE            = 56130,
    SPELL_BROOD_PLAGUE_H          = 59467,
    SPELL_BERSERK                 = 26662,
    SPELL_BROOD_RAGE              = 59465,

    SPELL_GUARDIAN_AURA           = 56151,

    // JustSummoned is not called for spell summoned creatures
    SPELL_SUMMON_SWARM_GUARDIAN   = 56120,
    SPELL_SUMMON_SWARMERS         = 56119,

    NPC_AHNKAHAR_GUARDIAN         = 30176,
    NPC_AHNKAHAR_SWARMER          = 30178
};

/*######
## mob_ahnkahat_egg
######*/
struct mob_ahnkahar_eggAI : public ScriptedAI
{
    mob_ahnkahar_eggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}

    void JustSummoned(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        if (pSummoned->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
        {
            pSummoned->CastSpell(pSummoned, SPELL_GUARDIAN_AURA, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(EMOTE_HATCH, m_creature);
        }

        if (Creature* pElderNadox = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_NADOX))
        {
            if (pElderNadox->GetVictim())
                pSummoned->AI()->AttackStart(pElderNadox->GetVictim());
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // If the Guardian is killed set the achiev criteria to false
        if (pSummoned->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
        {
            if (m_pInstance)
                m_pInstance->SetData(TYPE_NADOX, SPECIAL);
        }
    }
};

UnitAI* GetAI_mob_ahnkahar_egg(Creature* pCreature)
{
    return new mob_ahnkahar_eggAI(pCreature);
}

/*######
## boss_nadox
######*/

struct boss_nadoxAI : public ScriptedAI
{
    boss_nadoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    bool   m_bBerserk;
    bool   m_bGuardianSummoned;
    uint32 m_uiBroodPlagueTimer;
    uint32 m_uiBroodRageTimer;
    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_bBerserk = false;
        m_bGuardianSummoned = false;
        m_uiSummonTimer = 5000;
        m_uiBroodPlagueTimer = 15000;
        m_uiBroodRageTimer = 20000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bGuardianSummoned && m_creature->GetHealthPercent() < 50.0f)
        {
            // guardian is summoned at 50% of boss HP
            if (m_pInstance)
            {
                if (Creature* pGuardianEgg = m_creature->GetMap()->GetCreature(m_pInstance->SelectRandomGuardianEggGuid()))
                    pGuardianEgg->CastSpell(pGuardianEgg, SPELL_SUMMON_SWARM_GUARDIAN, TRIGGERED_NONE);

                m_bGuardianSummoned = true;
            }
        }

        if (m_uiSummonTimer < uiDiff)
        {
            if (roll_chance_i(50))
                DoScriptText(urand(0, 1) ? SAY_SUMMON_EGG_1 : SAY_SUMMON_EGG_2, m_creature);

            if (m_pInstance)
            {
                // There are 2 Swarmers summoned at a timer
                if (Creature* pSwarmerEgg = m_creature->GetMap()->GetCreature(m_pInstance->SelectRandomSwarmerEggGuid()))
                {
                    for (uint8 i = 0; i < 2; ++i)
                        pSwarmerEgg->CastSpell(pSwarmerEgg, SPELL_SUMMON_SWARMERS, TRIGGERED_NONE);
                }
            }

            m_uiSummonTimer = urand(5000, 10000);
        }
        else
            m_uiSummonTimer -= uiDiff;

        if (m_uiBroodPlagueTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BROOD_PLAGUE : SPELL_BROOD_PLAGUE_H);

            m_uiBroodPlagueTimer = 20000;
        }
        else
            m_uiBroodPlagueTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiBroodRageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BROOD_RAGE) == CAST_OK)
                    m_uiBroodRageTimer = 20000;
            }
            else
                m_uiBroodRageTimer -= uiDiff;
        }

        if (!m_bBerserk && m_creature->GetPositionZ() < 24.0)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                m_bBerserk = true;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_nadox(Creature* pCreature)
{
    return new boss_nadoxAI(pCreature);
}

void AddSC_boss_nadox()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nadox";
    pNewScript->GetAI = &GetAI_boss_nadox;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ahnkahar_egg";
    pNewScript->GetAI = &GetAI_mob_ahnkahar_egg;
    pNewScript->RegisterSelf();
}
