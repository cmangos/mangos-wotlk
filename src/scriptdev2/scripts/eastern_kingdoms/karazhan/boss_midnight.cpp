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
SDName: Boss_Midnight
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    SAY_MIDNIGHT_KILL           = -1532000,
    SAY_APPEAR1                 = -1532001,
    SAY_APPEAR2                 = -1532002,
    SAY_APPEAR3                 = -1532003,
    SAY_MOUNT                   = -1532004,
    SAY_KILL1                   = -1532005,
    SAY_KILL2                   = -1532006,
    SAY_DISARMED                = -1532007,
    SAY_DEATH                   = -1532008,
    SAY_RANDOM1                 = -1532009,
    SAY_RANDOM2                 = -1532010,

    // Midnight
    SPELL_MOUNT                 = 29770,
    SPELL_KNOCKDOWN             = 29711,
    SPELL_SUMMON_ATTUMEN        = 29714,
    SPELL_SUMMON_ATTUMEN_MOUNTED = 29799,

    // Attumen
    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_UPPERCUT              = 29850,
    SPELL_BERSERKER_CHARGE      = 26561,                    // Only when mounted

    NPC_ATTUMEN_MOUNTED         = 16152,
};

struct boss_midnightAI : public ScriptedAI
{
    boss_midnightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiKnockDown;

    void Reset() override
    {
        m_uiPhase     = 0;
        m_uiKnockDown = urand(6000, 9000);

        SetCombatMovement(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ATTUMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (m_uiPhase == 1 && m_pInstance)
        {
            if (Creature* pAttumen = m_pInstance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
                DoScriptText(SAY_MIDNIGHT_KILL, pAttumen);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ATTUMEN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());

        if (pSummoned->GetEntry() == NPC_ATTUMEN)
        {
            // Attumen yells when spawned
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_APPEAR1, pSummoned); break;
                case 1: DoScriptText(SAY_APPEAR2, pSummoned); break;
                case 2: DoScriptText(SAY_APPEAR3, pSummoned); break;
            }
        }
        else if (pSummoned->GetEntry() == NPC_ATTUMEN_MOUNTED)
        {
            DoScriptText(SAY_MOUNT, pSummoned);

            if (!m_pInstance)
                return;

            // The summoned has the health equal to the one which has the higher HP percentage of both
            if (Creature* pAttumen = m_pInstance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
                pSummoned->SetHealth(pAttumen->GetHealth() > m_creature->GetHealth() ? pAttumen->GetHealth() : m_creature->GetHealth());
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId || !m_pInstance)
            return;

        // Spawn the mounted Attumen and despawn
        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ATTUMEN_MOUNTED, CAST_TRIGGERED) == CAST_OK)
        {
            if (Creature* pAttumen = m_pInstance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
                pAttumen->ForcedDespawn();

            m_creature->ForcedDespawn();
        }
    }

    // Wrapper to prepare phase 3
    void DoPrepareMount(Creature* pTarget)
    {
        if (pTarget)
        {
            m_uiPhase = 2;

            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MovePoint(1, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Stop attacking during the mount phase
        if (m_uiPhase == 2)
            return;

        // Spawn Attumen on 95% hp
        if (m_uiPhase == 0 && m_creature->GetHealthPercent() < 95.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ATTUMEN) == CAST_OK)
                m_uiPhase = 1;
        }

        // Spawn Attumen mounted at 25%
        if (m_uiPhase == 1 && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MOUNT, CAST_TRIGGERED) == CAST_OK)
                m_uiPhase = 2;
        }

        if (m_uiKnockDown < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKDOWN) == CAST_OK)
                m_uiKnockDown = urand(6000, 9000);
        }
        else
            m_uiKnockDown -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_midnight(Creature* pCreature)
{
    return new boss_midnightAI(pCreature);
}

struct boss_attumenAI : public ScriptedAI
{
    boss_attumenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiRandomYellTimer;
    uint32 m_uiKnockDown;
    uint32 m_uiChargeTimer;                                 // only when mounted

    bool m_bHasSummonRider;

    void Reset() override
    {
        m_uiCleaveTimer     = urand(10000, 16000);
        m_uiCurseTimer      = 30000;
        m_uiRandomYellTimer = urand(30000, 60000);          // Occasionally yell
        m_uiChargeTimer     = 20000;
        m_uiKnockDown       = urand(6000, 9000);

        m_bHasSummonRider   = false;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void SpellHit(Unit* /*pSource*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Mechanic == MECHANIC_DISARM)
            DoScriptText(SAY_DISARMED, m_creature);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ATTUMEN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ATTUMEN, FAIL);

        // Despawn Attumen on fail
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOWCLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(10000, 16000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiCurseTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INTANGIBLE_PRESENCE) == CAST_OK)
                m_uiCurseTimer = 30000;
        }
        else
            m_uiCurseTimer -= uiDiff;

        if (m_uiRandomYellTimer < uiDiff)
        {
            DoScriptText(urand(0, 1) ? SAY_RANDOM1 : SAY_RANDOM2, m_creature);
            m_uiRandomYellTimer = urand(30000, 60000);
        }
        else
            m_uiRandomYellTimer -= uiDiff;

        if (m_uiKnockDown < uiDiff)
        {
            // Cast knockdown when mounted, otherwise uppercut
            if (DoCastSpellIfCan(m_creature->getVictim(), m_creature->GetEntry() == NPC_ATTUMEN_MOUNTED ? SPELL_KNOCKDOWN : SPELL_UPPERCUT) == CAST_OK)
                m_uiKnockDown = urand(6000, 9000);
        }
        else
            m_uiKnockDown -= uiDiff;

        // If creature is mounted then cast charge
        if (m_creature->GetEntry() == NPC_ATTUMEN_MOUNTED)
        {
            if (m_uiChargeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BERSERKER_CHARGE, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_BERSERKER_CHARGE) == CAST_OK)
                        m_uiChargeTimer = 20000;
                }
                else
                    m_uiChargeTimer = 2000;
            }
            else
                m_uiChargeTimer -= uiDiff;
        }
        // Else, mount if below 25%
        else if (!m_bHasSummonRider && m_creature->GetHealthPercent() < 25.0f)
        {
            if (Creature* pMidnight = m_pInstance->GetSingleCreatureFromStorage(NPC_MIDNIGHT))
            {
                pMidnight->CastSpell(m_creature, SPELL_MOUNT, true);
                m_bHasSummonRider = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_attumen(Creature* pCreature)
{
    return new boss_attumenAI(pCreature);
}

bool EffectDummyCreature_spell_mount_attumen(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_MOUNT && uiEffIndex == EFFECT_INDEX_0)
    {
        // Avoid possible DB errors
        if (pCaster->GetEntry() == NPC_MIDNIGHT && pCreatureTarget->GetEntry() == NPC_ATTUMEN)
        {
            // Prepare for mount
            if (boss_midnightAI* pMidnightAI = dynamic_cast<boss_midnightAI*>(((Creature*)pCaster)->AI()))
                pMidnightAI->DoPrepareMount(pCreatureTarget);
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

void AddSC_boss_attumen()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_attumen";
    pNewScript->GetAI = &GetAI_boss_attumen;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_mount_attumen;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_midnight";
    pNewScript->GetAI = &GetAI_boss_midnight;
    pNewScript->RegisterSelf();
}
