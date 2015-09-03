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
SDName: Boss_Warlord_Kalithres
SD%Complete: 90
SDComment: Timers may need some fine adjustments
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "precompiled.h"
#include "steam_vault.h"

enum
{
    SAY_INTRO                   = -1545016,
    SAY_REGEN                   = -1545017,
    SAY_AGGRO1                  = -1545018,
    SAY_AGGRO2                  = -1545019,
    SAY_AGGRO3                  = -1545020,
    SAY_SLAY1                   = -1545021,
    SAY_SLAY2                   = -1545022,
    SAY_DEATH                   = -1545023,

    SPELL_SPELL_REFLECTION      = 31534,
    SPELL_IMPALE                = 39061,
    SPELL_WARLORDS_RAGE         = 37081,        // triggers 36453
    SPELL_WARLORDS_RAGE_NAGA    = 31543,        // triggers 37076
};

struct boss_warlord_kalithreshAI : public ScriptedAI
{
    boss_warlord_kalithreshAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_steam_vault*)pCreature->GetInstanceData();
        m_bHasTaunted = false;
        Reset();
    }

    instance_steam_vault* m_pInstance;

    uint32 m_uiReflectionTimer;
    uint32 m_uiImpaleTimer;
    uint32 m_uiRageTimer;
    uint32 m_uiRageCastTimer;

    ObjectGuid m_distillerGuid;

    bool m_bHasTaunted;

    void Reset() override
    {
        m_uiReflectionTimer = 15000;
        m_uiImpaleTimer     = urand(7000, 14000);
        m_uiRageTimer       = urand(15000, 20000);
        m_uiRageCastTimer   = 0;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_WARLORD_KALITHRESH, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_WARLORD_KALITHRESH, DONE);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && m_creature->IsWithinDistInMap(pWho, 40.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // There is a small delay between the point reach and the channeling start
        m_uiRageCastTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRageCastTimer)
        {
            if (m_uiRageCastTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WARLORDS_RAGE) == CAST_OK)
                {
                    DoScriptText(SAY_REGEN, m_creature);
                    SetCombatMovement(true);
                    m_uiRageCastTimer = 0;

                    // Also make the distiller cast
                    if (Creature* pDistiller = m_creature->GetMap()->GetCreature(m_distillerGuid))
                    {
                        pDistiller->CastSpell(pDistiller, SPELL_WARLORDS_RAGE_NAGA, true);
                        pDistiller->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
            }
            else
                m_uiRageCastTimer -= uiDiff;
        }

        // Move to closest distiller
        if (m_uiRageTimer < uiDiff)
        {
            if (Creature* pDistiller = GetClosestCreatureWithEntry(m_creature, NPC_NAGA_DISTILLER, 100.0f))
            {
                float fX, fY, fZ;
                pDistiller->GetContactPoint(m_creature, fX, fY, fZ, INTERACTION_DISTANCE);
                m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                SetCombatMovement(false);
                m_distillerGuid = pDistiller->GetObjectGuid();
            }

            m_uiRageTimer = urand(35000, 45000);
        }
        else
            m_uiRageTimer -= uiDiff;

        // Reflection_Timer
        if (m_uiReflectionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPELL_REFLECTION) == CAST_OK)
                m_uiReflectionTimer = 30000;
        }
        else
            m_uiReflectionTimer -= uiDiff;

        // Impale_Timer
        if (m_uiImpaleTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_IMPALE) == CAST_OK)
                    m_uiImpaleTimer = urand(7500, 12500);
            }
        }
        else
            m_uiImpaleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool EffectAuraDummy_spell_aura_dummy_warlord_rage(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_WARLORDS_RAGE && pAura->GetEffIndex() == EFFECT_INDEX_0)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            // Resume combat when the cast is finished or interrupted
            if (!bApply)
            {
                if (pTarget->getVictim())
                {
                    pTarget->GetMotionMaster()->MovementExpired();
                    pTarget->GetMotionMaster()->MoveChase(pTarget->getVictim());
                }
            }
        }
    }

    return true;
}

struct mob_naga_distillerAI : public Scripted_NoMovementAI
{
    mob_naga_distillerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_boss_warlord_kalithresh(Creature* pCreature)
{
    return new boss_warlord_kalithreshAI(pCreature);
}

CreatureAI* GetAI_mob_naga_distiller(Creature* pCreature)
{
    return new mob_naga_distillerAI(pCreature);
}

void AddSC_boss_warlord_kalithresh()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_warlord_kalithresh";
    pNewScript->GetAI = &GetAI_boss_warlord_kalithresh;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_warlord_rage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_naga_distiller";
    pNewScript->GetAI = &GetAI_mob_naga_distiller;
    pNewScript->RegisterSelf();
}
