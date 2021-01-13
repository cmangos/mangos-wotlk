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
SD%Complete: 90%
SDComment: check timers
SDCategory: Utgarde Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_keep.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO               = -1574000,
    SAY_FROSTTOMB           = -1574001,
    SAY_SKELETONS           = -1574002,
    SAY_KILL                = -1574003,
    SAY_DEATH               = -1574004,
    EMOTE_FROST_TOMB        = -1574021,
    EMOTE_RISE_SKELETON     = -1574024,

    // Boss Spells
    SPELL_SHADOWBOLT        = 43667,
    SPELL_SHADOWBOLT_H      = 59389,

    SPELL_SUMMON_FROST_TOMB = 42714,                        // cast by player directly
    SPELL_FROST_TOMB_TARGET = 42672,                        // cast by boss on target
    // SPELL_FROST_TOMB     = 48400,                        // cast by creature 23965 on the summoner
    SPELL_CUSTOM_WALK       = 55059,

    // Skeleton Spells
    SPELL_DECREPIFY         = 42702,
    SPELL_DECREPIFY_H       = 59397,
    SPELL_BONE_ARMOR        = 59386,                        // casted on boss, heroic only (has script target)
    SPELL_SHADOW_FISSURE    = 50657,                        // cosmetic spell
    SPELL_FULL_HEAL         = 17683,

    NPC_VRYKUL_SKELETON     = 23970
};

static float fAddPosition[4] = {163.5727f, 252.1900f, 42.8684f, 5.57052f};

/*######
## mob_vrykul_skeleton
######*/

struct mob_vrykul_skeletonAI : public ScriptedAI
{
    mob_vrykul_skeletonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_utgarde_keep*>(pCreature->GetInstanceData());
        m_bIsRegularMode = m_creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_utgarde_keep* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiCastTimer;
    uint32 m_uiReviveTimer;
    uint32 m_uiAttackTimer;

    void Reset() override
    {
        m_uiReviveTimer = 0;
        m_uiAttackTimer = 6000;
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
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_FISSURE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL, CAST_TRIGGERED);
        DoScriptText(EMOTE_RISE_SKELETON, m_creature);

        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoResetThreat();
        m_uiAttackTimer = 1000;
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // ignore damage done by self
        if (pDealer == m_creature)
            return;

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
            m_creature->SetTarget(nullptr);
            m_creature->SetHealth(0);
            m_creature->RemoveAllAurasOnDeath();
            m_creature->GetMotionMaster()->Clear();
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAttackTimer)
        {
            if (m_uiAttackTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pKeleseth = m_pInstance->GetSingleCreatureFromStorage(NPC_KELESETH))
                        AttackStart(pKeleseth->GetVictim());
                }

                m_uiCastTimer = urand(5000, 10000);
                m_uiAttackTimer = 0;
            }
            else
                m_uiAttackTimer -= uiDiff;

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiReviveTimer)
        {
            if (m_uiReviveTimer <= uiDiff)
            {
                Revive();
                m_uiReviveTimer = 0;
            }
            else
                m_uiReviveTimer -= uiDiff;

            return;
        }

        if (m_uiCastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_DECREPIFY : SPELL_DECREPIFY_H) == CAST_OK)
            {
                if (!m_bIsRegularMode && urand(0, 1))
                    DoCastSpellIfCan(nullptr, SPELL_BONE_ARMOR, CAST_TRIGGERED);

                m_uiCastTimer = urand(5000, 15000);
            }
        }
        else
            m_uiCastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_keleseth
######*/

struct boss_kelesethAI : public ScriptedAI
{
    boss_kelesethAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_utgarde_keep*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_utgarde_keep* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFrostTombTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiShadowboltTimer;

    void Reset() override
    {
        // timers need confirmation
        m_uiFrostTombTimer = 20000;
        m_uiSummonTimer = 5000 ;
        m_uiShadowboltTimer = 0;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho, ATTACK_DISTANCE * 4);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_CUSTOM_WALK);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELESETH, IN_PROGRESS);
    }

    void SummonAdds()
    {
        for (uint8 i = 0; i < 4; ++i)
            m_creature->SummonCreature(NPC_VRYKUL_SKELETON, fAddPosition[0] + urand() % 7, fAddPosition[1] + urand() % 7, fAddPosition[2], fAddPosition[3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELESETH, DONE);
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
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_SHADOWBOLT : SPELL_SHADOWBOLT_H) == CAST_OK)
                m_uiShadowboltTimer = 3000;
        }
        else
            m_uiShadowboltTimer -= uiDiff;

        if (m_uiFrostTombTimer < uiDiff)
        {
            if (Unit* pTombTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SUMMON_FROST_TOMB, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTombTarget, SPELL_FROST_TOMB_TARGET) == CAST_OK)
                {
                    DoScriptText(SAY_FROSTTOMB, m_creature);
                    DoScriptText(EMOTE_FROST_TOMB, m_creature, pTombTarget);
                    m_uiFrostTombTimer = 25000;
                }
            }
        }
        else
            m_uiFrostTombTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## spell_frost_tomb_aura - 42672
######*/

struct spell_frost_tomb_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (aura->GetAuraTicks() == 2)
            target->CastSpell(target, 42714, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_keleseth()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_keleseth";
    pNewScript->GetAI = &GetNewAIInstance<boss_kelesethAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_vrykul_skeleton";
    pNewScript->GetAI = &GetNewAIInstance<mob_vrykul_skeletonAI>;
    pNewScript->RegisterSelf();

    RegisterAuraScript<spell_frost_tomb_aura>("spell_frost_tomb_aura");
}
