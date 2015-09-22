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
SDName: Boss_Keleseth
SD%Complete: 60%
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

#include "precompiled.h"
#include "utgarde_keep.h"

enum
{
    SAY_AGGRO               = -1574000,
    SAY_FROSTTOMB           = -1574001,
    SAY_SKELETONS           = -1574002,
    SAY_KILL                = -1574003,
    SAY_DEATH               = -1574004,
    EMOTE_FROST_TOMB        = -1574021,

    // Boss Spells
    SPELL_SHADOWBOLT        = 43667,
    SPELL_SHADOWBOLT_H      = 59389,

    SPELL_SUMMON_FROST_TOMB = 42714,
    SPELL_FROST_TOMB        = 48400,                        // stun and deal damage

    // Skeleton Spells
    SPELL_DECREPIFY         = 42702,
    SPELL_DECREPIFY_H       = 59397,
    SPELL_BONE_ARMOR        = 59386,                        // casted on boss, heroic only

    NPC_VRYKUL_SKELETON     = 23970
};

const float RUN_DISTANCE = 20.0;

static float fAddPosition[4] = {163.5727f, 252.1900f, 42.8684f, 5.57052f};

/*######
## mob_vrykul_skeleton
######*/

struct mob_vrykul_skeletonAI : public ScriptedAI
{
    mob_vrykul_skeletonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = m_creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiCastTimer;
    uint32 m_uiReviveTimer;

    void Reset() override
    {
        m_uiReviveTimer = 0;
        m_uiCastTimer = urand(5000, 10000);                 // taken out of thin air
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!pWho || m_uiReviveTimer)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (!pWho || m_uiReviveTimer)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Revive()
    {
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            m_creature->GetMotionMaster()->MoveChase(pTarget);

        DoResetThreat();
        m_uiReviveTimer = 0;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        if (m_uiReviveTimer)
        {
            uiDamage = 0;
            return;
        }

        if (m_creature->GetHealth() < uiDamage)
        {
            // start faking death
            uiDamage = 0;
            m_uiReviveTimer = 6000;
            m_creature->SetHealth(0);
            m_creature->RemoveAllAurasOnDeath();
            m_creature->GetMotionMaster()->Clear();
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiReviveTimer)
        {
            if (m_uiReviveTimer <= uiDiff)
                Revive();
            else
                m_uiReviveTimer -= uiDiff;

            return;
        }

        if (m_uiCastTimer < uiDiff)
        {
            if (m_bIsRegularMode)
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DECREPIFY);
            else
            {
                if (urand(0, 3))
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_DECREPIFY_H);
                else if (m_pInstance && m_pInstance->GetData(TYPE_KELESETH) == IN_PROGRESS)
                {
                    if (Creature* pKeleseth = m_pInstance->GetSingleCreatureFromStorage(NPC_KELESETH))
                        DoCastSpellIfCan(pKeleseth, SPELL_BONE_ARMOR);
                }
            }

            m_uiCastTimer = urand(5000, 15000);
        }
        else
            m_uiCastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_vrykul_skeleton(Creature* pCreature)
{
    return new mob_vrykul_skeletonAI(pCreature);
}

/*######
## boss_keleseth
######*/

struct boss_kelesethAI : public ScriptedAI
{
    boss_kelesethAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFrostTombTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiShadowboltTimer;

    GuidList m_lSummonedAddGuids;

    void Reset() override
    {
        // timers need confirmation
        m_uiFrostTombTimer = 20000;
        m_uiSummonTimer = 5000 ;
        m_uiShadowboltTimer = 0;

        DespawnOrKillAdds(true);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho, RUN_DISTANCE);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELESETH, IN_PROGRESS);
    }

    void SummonAdds()
    {
        for (uint8 i = 0; i < 4; ++i)
            m_creature->SummonCreature(NPC_VRYKUL_SKELETON, fAddPosition[0] + rand() % 7, fAddPosition[1] + rand() % 7, fAddPosition[2], fAddPosition[3], TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void DespawnOrKillAdds(bool bDespawn)
    {
        for (GuidList::const_iterator itr = m_lSummonedAddGuids.begin(); itr != m_lSummonedAddGuids.end(); ++itr)
        {
            if (Creature* pAdd = m_creature->GetMap()->GetCreature(*itr))
            {
                if (bDespawn)
                    pAdd->ForcedDespawn();
                else
                {
                    pAdd->SetDeathState(JUST_DIED);
                    pAdd->SetHealth(0);
                }
            }
        }

        m_lSummonedAddGuids.clear();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_VRYKUL_SKELETON)
        {
            pSummoned->AI()->AttackStart(m_creature->getVictim());
            m_lSummonedAddGuids.push_back(pSummoned->GetObjectGuid());
        }

        if (pSummoned->GetEntry() == NPC_FROST_TOMB)
            pSummoned->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FROST, true);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELESETH, DONE);

        DespawnOrKillAdds(false);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELESETH, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                SummonAdds();
                m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (m_uiShadowboltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOWBOLT : SPELL_SHADOWBOLT_H);
            m_uiShadowboltTimer = 3000;
        }
        else
            m_uiShadowboltTimer -= uiDiff;

        if (m_uiFrostTombTimer < uiDiff)
        {
            if (Unit* pTombTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                // DoCastSpellIfCan(pTombTarget, SPELL_SUMMON_FROST_TOMB);
                float fPosX, fPosY, fPosZ;
                pTombTarget->GetPosition(fPosX, fPosY, fPosZ);

                if (Creature* pFrostTomb = m_creature->SummonCreature(NPC_FROST_TOMB, fPosX, fPosY, fPosZ, 0, TEMPSUMMON_TIMED_DESPAWN, 20000))
                {
                    pFrostTomb->AddThreat(pTombTarget);
                    pFrostTomb->CastSpell(pTombTarget, SPELL_FROST_TOMB, false);
                }

                DoScriptText(SAY_FROSTTOMB, m_creature);
                DoScriptText(EMOTE_FROST_TOMB, m_creature, pTombTarget);
            }

            m_uiFrostTombTimer = 25000;
        }
        else
            m_uiFrostTombTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_keleseth(Creature* pCreature)
{
    return new boss_kelesethAI(pCreature);
}

void AddSC_boss_keleseth()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_keleseth";
    pNewScript->GetAI = &GetAI_boss_keleseth;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_vrykul_skeleton";
    pNewScript->GetAI = &GetAI_mob_vrykul_skeleton;
    pNewScript->RegisterSelf();
}
