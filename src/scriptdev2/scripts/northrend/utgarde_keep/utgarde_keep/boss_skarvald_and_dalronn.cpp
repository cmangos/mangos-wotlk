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
SDName: Boss_Skarvald_and_Dalronn
SD%Complete: 60%
SDComment: TODO: correct timers
SDCategory: Utgarde Keep
EndScriptData */

#include "precompiled.h"
#include "utgarde_keep.h"

enum
{
    SAY_SKA_AGGRO                       = -1574011,
    SAY_SKA_DEATH                       = -1574012,
    SAY_SKA_DEATH_REAL                  = -1574013,
    SAY_SKA_KILL                        = -1574014,
    SAY_SKA_DAL_DIES_REPLY              = -1574015,

    SAY_DAL_AGGRO_REPLY                 = -1574016,
    SAY_DAL_DEATH                       = -1574017,
    SAY_DAL_DEATH_REAL                  = -1574018,
    SAY_DAL_KILL                        = -1574019,
    SAY_DAL_SKA_DIES_REPLY              = -1574020,

    SPELL_SUMMON_DAL_GHOST              = 48612,
    SPELL_SUMMON_SKA_GHOST              = 48613,

    NPC_DAL_GHOST                       = 27389,
    NPC_SKA_GHOST                       = 27390,

    NPC_SKELETAL                        = 28878,            // summoned guardian in heroic

    // skarvald
    SPELL_CHARGE                        = 43651,
    SPELL_STONE_STRIKE                  = 48583,
    SPELL_ENRAGE                        = 48193,

    // dalronn
    SPELL_SHADOW_BOLT                   = 43649,
    SPELL_SHADOW_BOLT_H                 = 59575,

    SPELL_DEBILITATE                    = 43650,
    SPELL_DEBILITATE_H                  = 59577,

    SPELL_SUMMON_SKELETONS              = 52611
};

struct Yell
{
    int32   m_iTextId;
    int32   m_iTextReplyId;
};

Yell m_aYell[] =
{
    {SAY_SKA_AGGRO, SAY_DAL_AGGRO_REPLY},
    {SAY_SKA_DEATH, SAY_DAL_SKA_DIES_REPLY},
    {SAY_DAL_DEATH, SAY_SKA_DAL_DIES_REPLY}
};

struct boss_s_and_d_dummyAI : public ScriptedAI
{
    boss_s_and_d_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    ObjectGuid m_ghostGuid;

    Creature* GetBuddy()
    {
        if (!m_pInstance)
            return NULL;

        return m_pInstance->GetSingleCreatureFromStorage(m_creature->GetEntry() == NPC_DALRONN ? NPC_SKARVALD : NPC_DALRONN);
    }

    void Reset() override { }

    void JustReachedHome() override
    {
        if (Creature* pBuddy = GetBuddy())
        {
            if (pBuddy->isDead())
                pBuddy->Respawn();
        }

        if (Creature* pGhost = m_creature->GetMap()->GetCreature(m_ghostGuid))
        {
            if (pGhost->isAlive())
                pGhost->ForcedDespawn();
        }
    }

    void EnterCombat(Unit* pWho) override
    {
        if (!pWho)
            return;

        if (Creature* pBuddy = GetBuddy())
        {
            if (!pBuddy->getVictim())
                pBuddy->AI()->AttackStart(pWho);
        }

        Aggro(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // EventAI can probably handle ghosts
        if (pSummoned->GetEntry() == NPC_DAL_GHOST || pSummoned->GetEntry() == NPC_SKA_GHOST)
            m_ghostGuid = pSummoned->GetObjectGuid();

        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1);

        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Creature* pBuddy = GetBuddy())
        {
            if (pBuddy->isAlive())
            {
                DoScriptText(m_creature->GetEntry() == NPC_SKARVALD ? m_aYell[1].m_iTextId : m_aYell[2].m_iTextId, m_creature);
                DoScriptText(m_creature->GetEntry() == NPC_SKARVALD ? m_aYell[1].m_iTextReplyId : m_aYell[2].m_iTextReplyId, pBuddy);

                pBuddy->CastSpell(m_creature, m_creature->GetEntry() == NPC_SKARVALD ? SPELL_SUMMON_SKA_GHOST : SPELL_SUMMON_DAL_GHOST, true);

                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            else
            {
                if (Creature* pGhost = m_creature->GetMap()->GetCreature(m_ghostGuid))
                    pGhost->ForcedDespawn();

                pBuddy->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
        }
    }
};

/*######
## boss_skarvald
######*/

struct boss_skarvaldAI : public boss_s_and_d_dummyAI
{
    boss_skarvaldAI(Creature* pCreature) : boss_s_and_d_dummyAI(pCreature) { Reset(); }

    uint32 m_uiYellDelayTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiStoneStrikeTimer;

    void Reset() override
    {
        m_uiYellDelayTimer = 0;
        m_uiChargeTimer = urand(2000, 6000);
        m_uiEnrageTimer = 15000;
        m_uiStoneStrikeTimer = 8000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(m_aYell[0].m_iTextId, m_creature);
        m_uiYellDelayTimer = 5000;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_SKA_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiYellDelayTimer)
        {
            if (m_uiYellDelayTimer <= uiDiff)
            {
                if (Creature* pBuddy = GetBuddy())
                    DoScriptText(m_aYell[0].m_iTextReplyId, pBuddy);

                m_uiYellDelayTimer = 0;
            }
            else
                m_uiYellDelayTimer -= uiDiff;
        }

        if (m_uiChargeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCastSpellIfCan(pTarget, SPELL_CHARGE);

            m_uiChargeTimer = urand(8000, 16000);
        }
        else
            m_uiChargeTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            m_uiEnrageTimer = 20000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiStoneStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STONE_STRIKE);
            m_uiStoneStrikeTimer = urand(5000, 15000);
        }
        else
            m_uiStoneStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_skarvald(Creature* pCreature)
{
    return new boss_skarvaldAI(pCreature);
}

/*######
## boss_dalronn
######*/

struct boss_dalronnAI : public boss_s_and_d_dummyAI
{
    boss_dalronnAI(Creature* pCreature) : boss_s_and_d_dummyAI(pCreature) { Reset(); }

    uint32 m_uiDebilitateTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiSkeletonTimer;

    void Reset() override
    {
        m_uiDebilitateTimer = urand(5000, 10000);
        m_uiShadowBoltTimer = urand(2500, 6000);
        m_uiSkeletonTimer = urand(25000, 35000);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DAL_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDebilitateTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_DEBILITATE : SPELL_DEBILITATE_H);

            m_uiDebilitateTimer = urand(12000, 20000);
        }
        else
            m_uiDebilitateTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H);

            m_uiShadowBoltTimer = urand(3000, 6000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiSkeletonTimer < uiDiff)
            {
                if (!m_creature->FindGuardianWithEntry(NPC_SKELETAL))
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_SKELETONS);

                m_uiSkeletonTimer = 30000;
            }
            else
                m_uiSkeletonTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dalronn(Creature* pCreature)
{
    return new boss_dalronnAI(pCreature);
}

void AddSC_boss_skarvald_and_dalronn()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_skarvald";
    pNewScript->GetAI = &GetAI_boss_skarvald;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_dalronn";
    pNewScript->GetAI = &GetAI_boss_dalronn;
    pNewScript->RegisterSelf();
}
