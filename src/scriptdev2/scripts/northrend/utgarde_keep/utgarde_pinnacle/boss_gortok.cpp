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
SDName: Boss_Gortok
SD%Complete: 90%
SDComment: Timers; The subbosses and Gortok should be activated on aura remove
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_DEATH               = -1575018,

    SPELL_FREEZE_ANIM       = 16245,

    SPELL_IMPALE            = 48261,
    SPELL_IMPALE_H          = 59268,

    SPELL_WITHERING_ROAR    = 48256,
    SPELL_WITHERING_ROAR_H  = 59267,

    SPELL_ARCING_SMASH      = 48260
};

/*######
## boss_gortok
######*/

struct boss_gortokAI : public ScriptedAI
{
    boss_gortokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiRoarTimer;
    uint32 m_uiImpaleTimer;
    uint32 m_uiArcingSmashTimer;

    void Reset() override
    {
        m_uiRoarTimer        = 10000;
        m_uiImpaleTimer      = 15000;
        m_uiArcingSmashTimer = urand(5000, 8000);

        // This needs to be reset in case the event fails
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
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

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WITHERING_ROAR : SPELL_WITHERING_ROAR_H) == CAST_OK)
                m_uiRoarTimer = 10000;
        }
        else
            m_uiRoarTimer -= uiDiff;

        if (m_uiImpaleTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H) == CAST_OK)
                    m_uiImpaleTimer = urand(8000, 15000);
            }
        }
        else
            m_uiImpaleTimer -= uiDiff;

        if (m_uiArcingSmashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCING_SMASH) == CAST_OK)
                m_uiArcingSmashTimer = urand(5000, 13000);
        }
        else
            m_uiArcingSmashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

bool EffectDummyCreature_spell_awaken_gortok(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_AWAKEN_GORTOK && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreatureTarget->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);

        // Start attacking the players
        if (instance_pinnacle* pInstance = (instance_pinnacle*)pCreatureTarget->GetInstanceData())
        {
            if (Unit* pStarter = pCreatureTarget->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                pCreatureTarget->AI()->AttackStart(pStarter);
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

bool EffectAuraDummy_spell_aura_dummy_awaken_subboss(const Aura* pAura, bool bApply)
{
    // Note: this should be handled on aura remove, but this can't be done because there are some core issues with areaeffect spells
    if (pAura->GetId() == SPELL_AWAKEN_SUBBOSS && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pTarget->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);

            // Start attacking the players
            if (instance_pinnacle* pInstance = (instance_pinnacle*)pTarget->GetInstanceData())
            {
                if (Unit* pStarter = pTarget->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                    pTarget->AI()->AttackStart(pStarter);
            }
        }
    }
    return true;
}

bool ProcessEventId_event_spell_gortok_event(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)((Creature*)pSource)->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS || pInstance->GetData(TYPE_GORTOK) == DONE)
            return false;

        pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
        pInstance->SetGortokEventStarter(pSource->GetObjectGuid());
        return true;
    }
    return false;
}

void AddSC_boss_gortok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gortok";
    pNewScript->GetAI = &GetAI_boss_gortok;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_awaken_gortok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gortok_subboss";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_awaken_subboss;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_gortok_event";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_gortok_event;
    pNewScript->RegisterSelf();
}
