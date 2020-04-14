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
SDName: Boss_Tharonja
SD%Complete: 80%
SDComment: Encounter mechanic is not verified, spell CLEAR_GIFT_OF_THARONJA need core support
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Globals/ObjectMgr.h"
#include "draktharon_keep.h"

enum
{
    SAY_AGGRO                       = -1600012,
    SAY_KILL_1                      = -1600013,
    SAY_KILL_2                      = -1600014,
    SAY_FLESH_1                     = -1600015,
    SAY_FLESH_2                     = -1600016,
    SAY_SKELETON_1                  = -1600017,
    SAY_SKELETON_2                  = -1600018,
    SAY_DEATH                       = -1600019,

    SPELL_CURSE_OF_LIFE             = 49527,
    SPELL_CURSE_OF_LIFE_H           = 59972,
    SPELL_RAIN_OF_FIRE              = 49518,
    SPELL_RAIN_OF_FIRE_H            = 59971,
    SPELL_SHADOW_VOLLEY             = 49528,
    SPELL_SHADOW_VOLLEY_H           = 59973,
    SPELL_LIGHTNING_BREATH          = 49537,
    SPELL_LIGHTNING_BREATH_H        = 59963,
    SPELL_EYE_BEAM                  = 49544,
    SPELL_EYE_BEAM_H                = 59965,
    SPELL_POISON_CLOUD              = 49548,
    SPELL_POISON_CLOUD_H            = 59969,
    SPELL_DECAY_FLESH               = 49356,                // Has also unknown dummy aura onto all players while transforming
    SPELL_GIFT_OF_THARONJA          = 52509,
    SPELL_RETURN_FLESH              = 53463,                // Has also unknown dummy aura onto all players while transforming
    SPELL_CLEAR_GIFT_OF_THARONJA    = 53242,

    SPELL_ACHIEVEMENT_CHECK         = 61863,                // Exact Purpose unknown, but is criteria (BE_SPELL_TARGET(28) and BE_SPELL_TARGET2(69))

    // Only used to change display ID, might infact be some sort of UpdateEntry - TODO, research!
    NPC_THARONJA_SKELETAL           = 26632,
    NPC_THARONJA_FLESH              = 27696,
};

enum Phases
{
    PHASE_SKELETAL                  = 0,
    PHASE_TAKE_FLESH                = 1,
    PHASE_FLESH                     = 2,
    PHASE_RETURN_FLESH              = 3,
    PHASE_SKELETAL_END              = 4,
};

/*######
## boss_tharonja
######*/

struct boss_tharonjaAI : public ScriptedAI
{
    boss_tharonjaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Phases m_uiPhase;

    uint32 m_uiCurseLifeTimer;
    uint32 m_uiRainFireTimer;
    uint32 m_uiShadowVolleyTimer;
    uint32 m_uiLightningBreathTimer;
    uint32 m_uiEyeBeamTimer;
    uint32 m_uiPoisonCloudTimer;
    uint32 m_uiReturnFleshTimer;

    void Reset() override
    {
        m_uiPhase = PHASE_SKELETAL;

        m_uiCurseLifeTimer = urand(15000, 20000);
        m_uiRainFireTimer = urand(16000, 23000);            // This timer is not very accurate
        m_uiShadowVolleyTimer = urand(8000, 10000);
        m_uiLightningBreathTimer = urand(3000, 4000);
        m_uiEyeBeamTimer = urand(15000, 18000);             // This timer is not very accurate
        m_uiPoisonCloudTimer = urand(9000, 11000);          // This timer is not very accurate
        m_uiReturnFleshTimer = 26000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THARONJA, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT_CHECK, CAST_TRIGGERED | CAST_FORCE_CAST);

        // TODO check if this spell casting is infact also needed on phase-switch or only here (possible that there is also some sort of hp% dependency
        if (m_uiPhase == PHASE_FLESH)
            DoCastSpellIfCan(m_creature, SPELL_CLEAR_GIFT_OF_THARONJA, CAST_TRIGGERED | CAST_FORCE_CAST);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THARONJA, DONE);
    }

    void JustReachedHome() override
    {
        // Reset Display ID
        if (CreatureInfo const* pCreatureInfo = GetCreatureTemplateStore(NPC_THARONJA_SKELETAL))
        {
            uint32 uiDisplayId = Creature::ChooseDisplayId(pCreatureInfo);
            if (m_creature->GetDisplayId() != uiDisplayId)
                m_creature->SetDisplayId(uiDisplayId);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THARONJA, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_SKELETAL:
                // Phase switching at 50% (was in older patch versions multiple times, but from 335 on only once)
                if (m_creature->GetHealthPercent() < 50)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DECAY_FLESH, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_FLESH_1 : SAY_FLESH_2, m_creature);
                        m_uiPhase = PHASE_TAKE_FLESH;

                        return;                             // return here, as there is nothing more to be done in this phase
                    }
                }

            // No break here, the last phase is exactly like the first, but he doesn't change anymore
            case PHASE_SKELETAL_END:
                if (m_uiCurseLifeTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CURSE_OF_LIFE : SPELL_CURSE_OF_LIFE_H) == CAST_OK)
                            m_uiCurseLifeTimer = urand(12000, 18000);
                }
                else
                    m_uiCurseLifeTimer -= uiDiff;

                if (m_uiRainFireTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_RAIN_OF_FIRE : SPELL_RAIN_OF_FIRE_H) == CAST_OK)
                            m_uiRainFireTimer = urand(22000, 29000);
                }
                else
                    m_uiRainFireTimer -= uiDiff;

                if (m_uiShadowVolleyTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_VOLLEY : SPELL_SHADOW_VOLLEY_H) == CAST_OK)
                        m_uiShadowVolleyTimer = urand(6000, 12000);
                }
                else
                    m_uiShadowVolleyTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;

            case PHASE_FLESH:
                // This is not entirely clear if this _might_ also be triggered HP-dependend
                if (m_uiReturnFleshTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_RETURN_FLESH, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_SKELETON_1 : SAY_SKELETON_2, m_creature);
                        m_uiReturnFleshTimer = 26000;
                        m_uiPhase = PHASE_RETURN_FLESH;

                        return;                             // return here, as there is nothing more to be done in this phase
                    }
                }
                else
                    m_uiReturnFleshTimer -= uiDiff;

                if (m_uiPoisonCloudTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_POISON_CLOUD : SPELL_POISON_CLOUD_H) == CAST_OK)
                            m_uiPoisonCloudTimer = urand(7000, 12000);
                }
                else
                    m_uiPoisonCloudTimer -= uiDiff;

                if (m_uiLightningBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_LIGHTNING_BREATH : SPELL_LIGHTNING_BREATH_H) == CAST_OK)
                        m_uiLightningBreathTimer = urand(5000, 8000);
                }
                else
                    m_uiLightningBreathTimer -= uiDiff;

                if (m_uiEyeBeamTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_EYE_BEAM : SPELL_EYE_BEAM_H) == CAST_OK)
                        m_uiEyeBeamTimer = urand(12000, 15000);
                }
                else
                    m_uiEyeBeamTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;

            case PHASE_TAKE_FLESH:
                // Turn players into skeletons
                if (DoCastSpellIfCan(m_creature, SPELL_GIFT_OF_THARONJA) == CAST_OK)
                {
                    // Change modell - might be UpdateEntry
                    if (CreatureInfo const* pCreatureInfo = GetCreatureTemplateStore(NPC_THARONJA_FLESH))
                    {
                        uint32 uiDisplayId = Creature::ChooseDisplayId(pCreatureInfo);
                        m_creature->SetDisplayId(uiDisplayId);
                    }

                    m_uiPhase = PHASE_FLESH;
                }
                break;

            case PHASE_RETURN_FLESH:
                // Turn players into normal
                if (DoCastSpellIfCan(m_creature, SPELL_CLEAR_GIFT_OF_THARONJA) == CAST_OK)
                {
                    // Change modell - might be UpdateEntry
                    if (CreatureInfo const* pCreatureInfo = GetCreatureTemplateStore(NPC_THARONJA_SKELETAL))
                    {
                        uint32 uiDisplayId = Creature::ChooseDisplayId(pCreatureInfo);
                        m_creature->SetDisplayId(uiDisplayId);
                    }

                    m_uiPhase = PHASE_SKELETAL_END;
                }
                break;
        }
    }
};

UnitAI* GetAI_boss_tharonja(Creature* pCreature)
{
    return new boss_tharonjaAI(pCreature);
}

void AddSC_boss_tharonja()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_tharonja";
    pNewScript->GetAI = &GetAI_boss_tharonja;
    pNewScript->RegisterSelf();
}
