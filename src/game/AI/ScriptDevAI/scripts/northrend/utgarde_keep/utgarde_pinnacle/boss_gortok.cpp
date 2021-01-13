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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"

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
        m_pInstance = static_cast<instance_pinnacle*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
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
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
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
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCING_SMASH) == CAST_OK)
                m_uiArcingSmashTimer = urand(5000, 13000);
        }
        else
            m_uiArcingSmashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool ProcessEventId_event_spell_gortok_event(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(static_cast<Creature*>(pSource)->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS || pInstance->GetData(TYPE_GORTOK) == DONE)
            return false;

        pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
        pInstance->SetGortokEventStarter(pSource->GetObjectGuid());
        return true;
    }
    return false;
}

/*######
## spell_awaken_subboss_aura - 47669
######*/

struct spell_awaken_subboss_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (apply)
        {
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            target->RemoveAurasDueToSpell(16245);

            // Start attacking the players
            if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(target->GetInstanceData()))
            {
                if (Unit* pStarter = target->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                    target->AI()->AttackStart(pStarter);
            }
        }
    }
};

/*######
## spell_awaken_gortok - 47670
######*/

struct spell_awaken_gortok : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        target->RemoveAurasDueToSpell(16245);

        // Start attacking the players
        if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(target->GetInstanceData()))
        {
            if (Unit* pStarter = target->GetMap()->GetUnit(pInstance->GetGortokEventStarter()))
                target->AI()->AttackStart(pStarter);
        }
    }
};

void AddSC_boss_gortok()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gortok";
    pNewScript->GetAI = &GetNewAIInstance<boss_gortokAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_gortok_event";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_gortok_event;
    pNewScript->RegisterSelf();

    RegisterAuraScript<spell_awaken_subboss_aura>("spell_awaken_subboss_aura");
    RegisterSpellScript<spell_awaken_gortok>("spell_awaken_gortok");
}
