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
SDName: Boss_Anubarak
SD%Complete: 80%
SDComment: Summoned creatures movement may need some adjustments - may be solved with movement maps
SDCategory: Azjol'Nerub
EndScriptData */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
    SAY_INTRO                       = -1601014,
    SAY_AGGRO                       = -1601015,
    SAY_KILL_1                      = -1601016,
    SAY_KILL_2                      = -1601017,
    SAY_KILL_3                      = -1601018,
    SAY_SUBMERGE_1                  = -1601019,
    SAY_SUBMERGE_2                  = -1601020,
    SAY_LOCUST_1                    = -1601021,
    SAY_LOCUST_2                    = -1601022,
    SAY_LOCUST_3                    = -1601023,
    SAY_DEATH                       = -1601024,

    SPELL_CARRION_BEETLES           = 53520,
    SPELL_LEECHING_SWARM            = 53467,
    SPELL_LEECHING_SWARM_H          = 59430,
    SPELL_IMPALE_AURA               = 53456,        // ticks at each 10 secs - summons 29184
    SPELL_POUND                     = 53472,
    SPELL_POUND_H                   = 59433,
    SPELL_SUBMERGE                  = 53421,
    SPELL_EMERGE                    = 53500,

    // NOTES:
    // The Assassin and Guardian summon spell should be 53609 and 53613
    // They are currently not used because of the ignore LoS issue in core
    SPELL_SUMMON_ASSASSIN           = 53610,        // summons 29214
    SPELL_SUMMON_GUARDIAN           = 53614,        // summons 29216
    SPELL_SUMMON_VENOMANCER         = 53615,        // summons 29217
    SPELL_SUMMON_DARTER             = 53599,        // summons 29213

    // impale spells
    SPELL_IMPALE_VISUAL             = 53455,
    SPELL_IMPALE                    = 53454,
    SPELL_IMPALE_H                  = 59446,

    NPC_ANUBAR_DARTER               = 29213,
    NPC_ANUBAR_ASSASSIN             = 29214,
    NPC_ANUBAR_GUARDIAN             = 29216,
    NPC_ANUBAR_VENOMANCER           = 29217,
    NPC_IMPALE_TARGET               = 29184,

    PHASE_GROUND                    = 1,
    PHASE_SUBMERGED                 = 2
};


/*######
## boss_anubarak
######*/

struct boss_anubarakAI : public ScriptedAI
{
    boss_anubarakAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_azjol_nerub*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bDoneIntro = false;
        Reset();
    }

    instance_azjol_nerub* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint8 m_uiSubmergePhase;

    uint32 m_uiCarrionBeetlesTimer;
    uint32 m_uiLeechingSwarmTimer;
    uint32 m_uiPoundTimer;
    uint32 m_uiEmergeTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiDarterTimer;
    bool m_bIsFirstWave;
    bool m_bDoneIntro;

    void Reset() override
    {
        m_uiPhase = PHASE_GROUND;
        m_uiSubmergePhase = 1;

        m_uiCarrionBeetlesTimer = 8000;
        m_uiLeechingSwarmTimer  = 20000;
        m_uiPoundTimer          = 15000;
        m_uiDarterTimer         = 5000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
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

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bDoneIntro && m_creature->IsWithinDistInMap(pWho, 60.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (!m_pInstance)
            return;

        switch (pSummoned->GetEntry())
        {
            case NPC_ANUBAR_GUARDIAN:
            case NPC_ANUBAR_VENOMANCER:
                pSummoned->SetWalk(false);
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetAnubTrigger()))
                    pSummoned->GetMotionMaster()->MovePoint(0, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ());
                break;
            case NPC_ANUBAR_DARTER:
            case NPC_ANUBAR_ASSASSIN:
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetAnubTrigger()))
                {
                    float fX, fY, fZ;
                    m_creature->GetRandomPoint(pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 15.0f, fX, fY, fZ);

                    pSummoned->SetWalk(false);
                    pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                }
                break;
            case NPC_IMPALE_TARGET:
                pSummoned->CastSpell(pSummoned, SPELL_IMPALE_VISUAL, true);
                break;
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == PHASE_GROUND)
        {
            if (m_uiLeechingSwarmTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LEECHING_SWARM : SPELL_LEECHING_SWARM_H) == CAST_OK)
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_LOCUST_1, m_creature); break;
                        case 1: DoScriptText(SAY_LOCUST_2, m_creature); break;
                        case 2: DoScriptText(SAY_LOCUST_3, m_creature); break;
                    }

                    m_uiLeechingSwarmTimer = 19000;
                }
            }
            else
                m_uiLeechingSwarmTimer -= uiDiff;

            if (m_uiCarrionBeetlesTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CARRION_BEETLES) == CAST_OK)
                    m_uiCarrionBeetlesTimer = 25000;
            }
            else
                m_uiCarrionBeetlesTimer -= uiDiff;

            if (m_uiPoundTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_POUND : SPELL_POUND_H) == CAST_OK)
                    m_uiPoundTimer = 16000;
            }
            else
                m_uiPoundTimer -= uiDiff;

            if (m_creature->GetHealthPercent() < 100 - 25 * m_uiSubmergePhase)
            {
                DoCastSpellIfCan(m_creature, SPELL_IMPALE_AURA, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SUBMERGE, CAST_TRIGGERED);
                DoScriptText(urand(0, 1) ? SAY_SUBMERGE_1 : SAY_SUBMERGE_2, m_creature);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                m_uiPhase = PHASE_SUBMERGED;
                m_bIsFirstWave = true;
                m_uiSummonTimer = 5000;

                // Emerge timers aren't the same. They depend on the submerge phase
                switch (m_uiSubmergePhase)
                {
                    case 1:
                        m_uiEmergeTimer = 20000;
                        break;
                    case 2:
                        m_uiEmergeTimer = 45000;
                        break;
                    case 3:
                        m_uiEmergeTimer = 50000;
                        break;
                }
                ++m_uiSubmergePhase;
            }

            DoMeleeAttackIfReady();
        }
        else if (m_uiPhase == PHASE_SUBMERGED)
        {
            if (m_uiSummonTimer < uiDiff)
            {
                if (!m_pInstance)
                    return;

                // Summon 2 Assassins
                for (uint8 i = 0; i < 2; ++i)
                {
                    if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetRandomAssassinTrigger()))
                        pTrigger->CastSpell(pTrigger, SPELL_SUMMON_ASSASSIN, true, NULL, NULL, m_creature->GetObjectGuid());
                }

                // on the first wave summon a guardian; on the second wave summon a venonmancer
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetGuardianTrigger()))
                {
                    pTrigger->CastSpell(pTrigger, m_bIsFirstWave ? SPELL_SUMMON_GUARDIAN : SPELL_SUMMON_VENOMANCER, true, NULL, NULL, m_creature->GetObjectGuid());
                    m_bIsFirstWave = false;
                }

                m_uiSummonTimer = 26000;
            }
            else
                m_uiSummonTimer -= uiDiff;

            // only on the last submerge phase
            if (m_uiSubmergePhase == 4)
            {
                if (m_uiDarterTimer < uiDiff)
                {
                    if (!m_pInstance)
                        return;

                    if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetDarterTrigger()))
                    {
                        pTrigger->CastSpell(pTrigger, SPELL_SUMMON_DARTER, true, NULL, NULL, m_creature->GetObjectGuid());
                        m_uiDarterTimer = urand(10000, 15000);
                    }
                }
                else
                    m_uiDarterTimer -= uiDiff;
            }

            if (m_uiEmergeTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_INTERRUPT_PREVIOUS);
                m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                m_creature->RemoveAurasDueToSpell(SPELL_IMPALE_AURA);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                m_uiPhase = PHASE_GROUND;
            }
            else
                m_uiEmergeTimer -= uiDiff;
        }

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_anubarak(Creature* pCreature)
{
    return new boss_anubarakAI(pCreature);
}

/*######
## npc_impale_target
######*/

struct npc_impale_targetAI : public Scripted_NoMovementAI
{
    npc_impale_targetAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiImpaleTimer;

    void Reset() override
    {
        m_uiImpaleTimer = 3000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiImpaleTimer)
        {
            if (m_uiImpaleTimer <= uiDiff)
            {
                if (!m_pInstance)
                    return;

                m_creature->RemoveAurasDueToSpell(SPELL_IMPALE_VISUAL);

                // The impale is cast by Anub on the impale target
                if (Creature* pAnub = m_pInstance->GetSingleCreatureFromStorage(NPC_ANUBARAK))
                    pAnub->CastSpell(m_creature, m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H, true);

                m_creature->ForcedDespawn(3000);
                m_uiImpaleTimer = 0;
            }
            else
                m_uiImpaleTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_impale_target(Creature* pCreature)
{
    return new npc_impale_targetAI(pCreature);
}

void AddSC_boss_anubarak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_anubarak";
    pNewScript->GetAI = &GetAI_boss_anubarak;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_impale_target";
    pNewScript->GetAI = &GetAI_npc_impale_target;
    pNewScript->RegisterSelf();
}
