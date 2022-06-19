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
SDName: boss_auriaya
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                           = -1603079,
    SAY_SLAY_1                          = -1603080,
    SAY_SLAY_2                          = -1603081,
    SAY_BERSERK                         = -1603082,
    SAY_DEATH                           = -1603083,
    EMOTE_SCREECH                       = -1603084,
    EMOTE_DEFENDER                      = -1603085,

    // Auriaya
    SPELL_BERSERK                       = 47008,
    SPELL_GUARDIAN_SWARM                = 64396,            // triggers 64397
    SPELL_SENTINEL_BLAST                = 64389,            // triggers 64392
    SPELL_SENTINEL_BLAST_H              = 64678,            // triggers 64679
    SPELL_SONIC_SCREECH                 = 64422,
    SPELL_SONIC_SCREECH_H               = 64688,
    SPELL_TERRIFYING_SCREECH            = 64386,
    SPELL_ACTIVATE_FERAL_DEFENDER       = 64449,            // triggers 64447
    SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG = 64448,

    // Feral Defender spells
    SPELL_FERAL_ESSENCE                 = 64455,
    SPELL_FERAL_ESSENCE_REMOVAL         = 64456,            // remove 1 stack of 64455
    SPELL_FERAL_POUNCE                  = 64478,
    SPELL_FERAL_POUNCE_H                = 64669,
    SPELL_FERAL_RUSH                    = 64489,            // triggers 64496
    SPELL_FERAL_RUSH_H                  = 64673,            // triggers 64674
    SPELL_SEEPING_FERAL_ESSENCE_SUMMON  = 64457,
    SPELL_FEIGN_DEATH                   = 64461,            // related to the feral defender feign death
    SPELL_FULL_HEAL                     = 64460,            // on feign death remove

    // Seeping Feral Essence
    SPELL_SEEPING_FERAL_ESSENCE         = 64458,
    SPELL_SEEPING_FERAL_ESSENCE_H       = 64676,

    NPC_SEEPING_FERAL_ESSENCE           = 34098,            // summoned by the feral defender on feign death
    // NPC_GUARDIAN_SWARN               = 34034,            // summoned by spell
    NPC_FERAL_DEFENDER_STALKER          = 34096,
};

/*######
## boss_auriaya
######*/

struct boss_auriayaAI : public ScriptedAI
{
    boss_auriayaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEnrageTimer;
    uint32 m_uiSwarmTimer;
    uint32 m_uiSonicScreechTimer;
    uint32 m_uiSentinelBlastTimer;
    uint32 m_uiTerrifyingScreechTimer;
    uint32 m_uiDefenderTimer;

    void Reset() override
    {
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;
        m_uiSwarmTimer              = 50000;
        m_uiSonicScreechTimer       = 58000;
        m_uiSentinelBlastTimer      = 40000;
        m_uiTerrifyingScreechTimer  = 38000;
        m_uiDefenderTimer           = 1 * MINUTE * IN_MILLISECONDS;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Summon the feral defender
        if (pSummoned->GetEntry() == NPC_FERAL_DEFENDER_STALKER)
            DoCastSpellIfCan(pSummoned, SPELL_ACTIVATE_FERAL_DEFENDER, CAST_INTERRUPT_PREVIOUS);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiTerrifyingScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_SCREECH) == CAST_OK)
            {
                DoScriptText(EMOTE_SCREECH, m_creature);
                m_uiTerrifyingScreechTimer = 35000;
                m_uiSentinelBlastTimer = 2000;
            }
        }
        else
            m_uiTerrifyingScreechTimer -= uiDiff;

        if (m_uiSentinelBlastTimer < uiDiff)
        {
            // Cast Sentinel blast right after terrifying screech
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SENTINEL_BLAST : SPELL_SENTINEL_BLAST_H) == CAST_OK)
                m_uiSentinelBlastTimer = 35000;
        }
        else
            m_uiSentinelBlastTimer -= uiDiff;

        if (m_uiDefenderTimer)
        {
            if (m_uiDefenderTimer <= uiDiff)
            {
                // Summon the feral defender trigger
                if (DoCastSpellIfCan(m_creature, SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG) == CAST_OK)
                    m_uiDefenderTimer = 0;
            }
            else
                m_uiDefenderTimer -= uiDiff;
        }

        if (m_uiSonicScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SONIC_SCREECH : SPELL_SONIC_SCREECH_H) == CAST_OK)
                m_uiSonicScreechTimer = 27000;
        }
        else
            m_uiSonicScreechTimer -= uiDiff;

        if (m_uiSwarmTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_GUARDIAN_SWARM) == CAST_OK)
                    m_uiSwarmTimer = 37000;
            }
        }
        else
            m_uiSwarmTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiEnrageTimer = 10 * MINUTE * IN_MILLISECONDS;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriayaAI(pCreature);
}

/*######
## boss_feral_defender
######*/

struct boss_feral_defenderAI : public ScriptedAI
{
    boss_feral_defenderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiMaxFeralRush = m_bIsRegularMode ? 6 : 10;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiFeralRushCount;
    uint8 m_uiMaxFeralRush;
    uint32 m_uiPounceTimer;
    uint32 m_uiFeralRushTimer;
    uint32 m_uiReviveDelayTimer;
    uint32 m_uiAttackDelayTimer;
    uint32 m_uiKilledCount;

    void Reset() override
    {
        m_uiFeralRushCount      = 0;
        m_uiReviveDelayTimer    = 0;
        m_uiAttackDelayTimer    = 0;
        m_uiPounceTimer         = urand(9000, 12000);
        m_uiFeralRushTimer      = urand(3000, 5000);
        m_uiKilledCount         = 0;

        DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // Set achiev criteria to true
        if (m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NINE_LIVES, true);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // If we don't have the feral essence anymore then ignore this
        if (m_uiKilledCount >= 8)                           // 9-1 == 8
            return;

        if (m_uiReviveDelayTimer)                           // Already faking
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            // Set Feign death, remove one aura stack and summon a feral essence
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE_REMOVAL, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SEEPING_FERAL_ESSENCE_SUMMON, CAST_TRIGGERED);

            // the feign death aura doesn't do everything, so keep the following here as well
            m_creature->SetHealth(0);
            m_creature->ClearComboPointHolders();
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->ClearAllReactives();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            m_uiReviveDelayTimer = 30000;
            ++m_uiKilledCount;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Cast seeping feral essence on the summoned
        if (pSummoned->GetEntry() == NPC_SEEPING_FERAL_ESSENCE)
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SEEPING_FERAL_ESSENCE : SPELL_SEEPING_FERAL_ESSENCE_H, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiReviveDelayTimer)
        {
            if (m_uiReviveDelayTimer <= uiDiff)
            {
                // ToDo: figure out how to enable the ressurrect animation
                DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL, CAST_TRIGGERED);
                m_uiReviveDelayTimer = 0;
                m_uiAttackDelayTimer = 3000;
            }
            else
                m_uiReviveDelayTimer -= uiDiff;

            // No Further action while faking
            return;
        }

        if (m_uiAttackDelayTimer)
        {
            if (m_uiAttackDelayTimer <= uiDiff)
            {
                DoResetThreat();
                m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

                m_uiPounceTimer = urand(9000, 10000);
                m_uiFeralRushCount = 0;
                m_uiFeralRushTimer = 1000;
                m_uiAttackDelayTimer = 0;
            }
            else
                m_uiAttackDelayTimer -= uiDiff;

            // No Further action while faking
            return;
        }

        if (m_uiPounceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FERAL_POUNCE : SPELL_FERAL_POUNCE_H) == CAST_OK)
                    m_uiPounceTimer = urand(13000, 16000);
            }
        }
        else
            m_uiPounceTimer -= uiDiff;

        if (m_uiFeralRushTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FERAL_RUSH : SPELL_FERAL_RUSH_H) == CAST_OK)
            {
                ++m_uiFeralRushCount;

                if (m_uiFeralRushCount == m_uiMaxFeralRush)
                {
                    m_uiFeralRushCount = 0;
                    m_uiFeralRushTimer = 12000;
                }
                else
                    m_uiFeralRushTimer = 400;
            }
        }
        else
            m_uiFeralRushTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_feral_defender(Creature* pCreature)
{
    return new boss_feral_defenderAI(pCreature);
}

void AddSC_boss_auriaya()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_auriaya";
    pNewScript->GetAI = GetAI_boss_auriaya;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_feral_defender";
    pNewScript->GetAI = &GetAI_boss_feral_defender;
    pNewScript->RegisterSelf();
}
