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
SDName: assembly_of_iron
SD%Complete: 90%
SDComment: Lightning Tendrils target following could use some love from the core side
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"

enum
{
    SAY_BRUNDIR_AGGRO                   = -1603056,
    SAY_BRUNDIR_WHIRL                   = -1603057,
    SAY_BRUNDIR_DEATH_1                 = -1603058,
    SAY_BRUNDIR_DEATH_2                 = -1603059,
    SAY_BRUNDIR_SLAY_1                  = -1603060,
    SAY_BRUNDIR_SLAY_2                  = -1603061,
    SAY_BRUNDIR_BERSERK                 = -1603062,
    SAY_BRUNDIR_FLY                     = -1603063,

    SAY_MOLGEIM_AGGRO                   = -1603064,
    SAY_MOLGEIM_DEATH_1                 = -1603065,
    SAY_MOLGEIM_DEATH_2                 = -1603066,
    SAY_MOLGEIM_DEATH_RUNE              = -1603067,
    SAY_MOLGEIM_SURGE                   = -1603068,
    SAY_MOLGEIM_SLAY_1                  = -1603069,
    SAY_MOLGEIM_SLAY_2                  = -1603070,
    SAY_MOLGEIM_BERSERK                 = -1603071,

    SAY_STEEL_AGGRO                     = -1603072,
    SAY_STEEL_DEATH_1                   = -1603073,
    SAY_STEEL_DEATH_2                   = -1603074,
    SAY_STEEL_SLAY_1                    = -1603075,
    SAY_STEEL_SLAY_2                    = -1603076,
    SAY_STEEL_OVERWHELM                 = -1603077,
    SAY_STEEL_BERSERK                   = -1603078,

    // Common spells
    SPELL_BERSERK                       = 62535,        // triggers 47008 after 15 min
    SPELL_SUPERCHARGE                   = 61920,
    SPELL_LIGHTNING_CHANNEL_PREFIGHT    = 61942,        // cast by Brundir on Steelbreaker
    SPELL_RUNE_OF_POWER_PREFIGHT        = 61975,        // cast by Molgeim on Stellbreaker
    SPELL_COUNCIL_KILL_CREDIT           = 65195,        // currently missing from DBC

    // Steelbreaker
    SPELL_HIGH_VOLTAGE                  = 61890,        // phase 1 spells
    SPELL_HIGH_VOLTAGE_H                = 63498,        // probably related to 61892 - couldn't find any info regarding this one
    SPELL_FUSION_PUNCH                  = 61903,
    SPELL_FUSION_PUNCH_H                = 63493,
    SPELL_STATIC_DISRUPTION             = 61911,        // phase 2 spells
    SPELL_STATIC_DISRUPTION_H           = 63495,        // should be triggered by 64641
    SPELL_OVERWHELMING_POWER            = 61888,        // phase 3 spells
    SPELL_OVERWHELMING_POWER_H          = 64637,
    SPELL_ELECTRICAL_CHARGE             = 61900,        // triggers 61901 when target dies

    // Runemaster Molgeim
    SPELL_SHIELD                        = 62274,        // phase 1 spells
    SPELL_SHIELD_H                      = 63489,
    SPELL_RUNE_OF_POWER                 = 61973,
    SPELL_RUNE_OF_DEATH                 = 62269,        // phase 2 spells
    SPELL_RUNE_OF_DEATH_H               = 63490,
    SPELL_RUNE_OF_SUMMONING             = 62273,        // phase 3 spells

    // Stormcaller Brundir
    SPELL_CHAIN_LIGHTNING               = 61879,        // phase 1 spells
    SPELL_CHAIN_LIGHTNING_H             = 63479,
    SPELL_OVERLOAD                      = 61869,
    SPELL_LIGHTNING_WHIRL               = 61915,        // phase 2 spells
    SPELL_LIGHTNING_WHIRL_H             = 63483,
    SPELL_LIGHTNING_WHIRL_DAMAGE        = 61916,        // used to check achiev criterias
    SPELL_LIGHTNING_WHIRL_DAMAGE_H      = 63482,
    SPELL_STORMSHIELD                   = 64187,        // phase 3 spells
    SPELL_LIGHTNING_TENDRILS            = 61887,
    SPELL_LIGHTNING_TENDRILS_H          = 63486,
    SPELL_TENDRILS_VISUAL               = 61883,

    // Summoned spells
    SPELL_OVERLOAD_AURA                 = 61877,
    SPELL_RUNE_OF_POWER_AURA            = 61974,
    SPELL_RUNE_OF_SUMMONING_AURA        = 62019,        // triggers 62020 which summons 32958
    SPELL_LIGHTNING_ELEMENTAL_PASSIVE   = 62052,
    SPELL_LIGHTNING_ELEMENTAL_PASSIVE_H = 63492,

    // summoned npcs
    NPC_OVERLOAD_VISUAL                 = 32866,
    NPC_RUNE_OF_POWER                   = 33705,
    NPC_RUNE_OF_SUMMONING               = 33051,
    NPC_LIGHTNING_ELEMENTAL             = 32958,

    PHASE_NO_CHARGE                     = 0,
    PHASE_CHARGE_ONE                    = 1,
    PHASE_CHARGE_TWO                    = 2,

    POINT_ID_LIFT_OFF                   = 1,
    POINT_ID_LAND                       = 2,
};

struct boss_brundirAI : public ScriptedAI
{
    boss_brundirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint32 m_uiVisualTimer;
    uint32 m_uiChainLightningTimer;
    uint32 m_uiOverloadTimer;
    uint32 m_uiWhirlTimer;
    uint32 m_uiTendrilsTimer;
    uint32 m_uiTendrilsTargetTimer;
    uint32 m_uiTendrilsEndTimer;
    uint32 m_uiTendrilsFollowTimer;

    ObjectGuid m_followTargetGuid;

    void Reset() override
    {
        m_uiPhase               = PHASE_NO_CHARGE;
        m_uiVisualTimer         = 5000;
        m_uiChainLightningTimer = 0;
        m_uiOverloadTimer       = 35000;
        m_uiWhirlTimer          = 10000;
        m_uiTendrilsTimer       = 60000;
        m_uiTendrilsEndTimer    = 0;
        m_uiTendrilsTargetTimer = 0;
        m_uiTendrilsFollowTimer = 500;

        m_creature->SetLevitate(false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // If we are not on the last phase then cast Supercharge and set as unlootable
        if (m_uiPhase != PHASE_CHARGE_TWO)
        {
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCastSpellIfCan(m_creature, SPELL_SUPERCHARGE, CAST_TRIGGERED);
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_BRUNDIR, false);
        }
        else
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
            m_creature->CastSpell(m_creature, SPELL_COUNCIL_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }

        DoScriptText(urand(0, 1) ? SAY_BRUNDIR_DEATH_1 : SAY_BRUNDIR_DEATH_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_BRUNDIR_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        m_creature->InterruptNonMeleeSpells(false);
        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_BRUNDIR_SLAY_1 : SAY_BRUNDIR_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_OVERLOAD_VISUAL)
        {
            pSummoned->CastSpell(pSummoned, SPELL_OVERLOAD_AURA, TRIGGERED_OLD_TRIGGERED);
            // Visual npc- shouldn't move and should despawn in 6 sec
            pSummoned->GetMotionMaster()->MoveIdle();
            pSummoned->ForcedDespawn(6000);
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // Increase the phase when hit with the supercharge spell by his brothers
        if (pSpell->Id == SPELL_SUPERCHARGE)
        {
            // Not sure if there is a spell for this, so we are doing it here
            m_creature->SetHealth(m_creature->GetMaxHealth());
            ++m_uiPhase;
        }

        if (m_uiPhase == PHASE_CHARGE_TWO)
        {
            // Cast stormshield in the last phase
            DoCastSpellIfCan(m_creature, SPELL_STORMSHIELD, CAST_TRIGGERED);

            // set the instace data to special in order to mark the last phase - this is used to check the achiev criteria
            if (m_pInstance)
                m_pInstance->SetData(TYPE_ASSEMBLY, SPECIAL);
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_pInstance)
            return;

        // Check achiev criterias
        switch (pSpell->Id)
        {
            case SPELL_CHAIN_LIGHTNING:
            case SPELL_CHAIN_LIGHTNING_H:
            case SPELL_LIGHTNING_WHIRL_DAMAGE:
            case SPELL_LIGHTNING_WHIRL_DAMAGE_H:
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_STUNNED, false);
                break;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        switch (uiPointId)
        {
            // After lift up follow a target and set the target change timer
            case POINT_ID_LIFT_OFF:
                // TODO: the boss should follow without changing his Z position - missing core feature
                // Current implementation with move point is wrong
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                {
                    DoMoveToTarget(pTarget);
                    m_followTargetGuid = pTarget->GetObjectGuid();
                }
                m_uiTendrilsTargetTimer = 5000;
                m_uiTendrilsFollowTimer = 500;
                break;
            // After reached the land remove all the auras and resume basic combat
            case POINT_ID_LAND:
                m_creature->SetLevitate(false);
                SetCombatMovement(true);
                if (m_creature->GetVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

                m_creature->RemoveAurasDueToSpell(SPELL_TENDRILS_VISUAL);
                m_creature->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_LIGHTNING_TENDRILS : SPELL_LIGHTNING_TENDRILS_H);
                break;
        }
    }

    // Wrapper for target movement
    void DoMoveToTarget(Unit* pTarget)
    {
        if (pTarget)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(0, pTarget->GetPositionX(), pTarget->GetPositionY(), m_creature->GetPositionZ());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Pre fight visual spell
        if (m_uiVisualTimer)
        {
            if (m_uiVisualTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_CHANNEL_PREFIGHT) == CAST_OK)
                    m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_CHARGE_TWO:

                if (m_uiTendrilsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_TENDRILS : SPELL_LIGHTNING_TENDRILS_H) == CAST_OK)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_TENDRILS_VISUAL, CAST_TRIGGERED);
                        DoScriptText(SAY_BRUNDIR_FLY, m_creature);
                        SetCombatMovement(false);
                        m_creature->SetLevitate(true);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_LIFT_OFF, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 15.0f);
                        m_uiTendrilsTimer = 90000;
                        m_uiTendrilsEndTimer = 25000;
                    }
                }
                else
                    m_uiTendrilsTimer -= uiDiff;

                if (m_uiTendrilsEndTimer)
                {
                    if (m_uiTendrilsEndTimer <= uiDiff)
                    {
                        // Get proper Z position and land
                        float groundZ = m_creature->GetMap()->GetHeight(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), false);
                        float fZ = m_creature->GetTerrain()->GetWaterOrGroundLevel(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), groundZ);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_creature->GetPositionX(), m_creature->GetPositionY(), fZ);
                        m_uiOverloadTimer       = 40000;
                        m_uiWhirlTimer          = 15000;
                        m_uiChainLightningTimer = 3000;
                        m_uiTendrilsEndTimer    = 0;
                        m_uiTendrilsTargetTimer = 0;
                    }
                    else
                        m_uiTendrilsEndTimer -= uiDiff;

                    // Change follow target every 5 seconds
                    if (m_uiTendrilsTargetTimer)
                    {
                        if (m_uiTendrilsTargetTimer <= uiDiff)
                        {
                            // TODO: the boss should follow without changing his Z position - missing core feature
                            // Current implementation with move point is wrong
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                            {
                                DoMoveToTarget(pTarget);
                                m_followTargetGuid = pTarget->GetObjectGuid();
                            }
                            m_uiTendrilsTargetTimer = 5000;
                            m_uiTendrilsFollowTimer = 500;
                        }
                        else
                            m_uiTendrilsTargetTimer -= uiDiff;

                        // Workaround to follow the target
                        if (m_uiTendrilsFollowTimer < uiDiff)
                        {
                            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_followTargetGuid))
                                DoMoveToTarget(pTarget);
                            m_uiTendrilsFollowTimer = 500;
                        }
                        else
                            m_uiTendrilsFollowTimer -= uiDiff;
                    }

                    // no other spells during tendrils
                    return;
                }

            // no break here; he uses the other spells as well
            case PHASE_CHARGE_ONE:

                if (m_uiWhirlTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_WHIRL : SPELL_LIGHTNING_WHIRL_H) == CAST_OK)
                    {
                        DoScriptText(SAY_BRUNDIR_WHIRL, m_creature);
                        m_uiWhirlTimer = 30000;
                    }
                }
                else
                    m_uiWhirlTimer -= uiDiff;

            // no break here; he uses the other spells as well
            case PHASE_NO_CHARGE:

                if (m_uiChainLightningTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                            m_uiChainLightningTimer = 2000;
                    }
                }
                else
                    m_uiChainLightningTimer -= uiDiff;

                if (m_uiOverloadTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_OVERLOAD) == CAST_OK)
                        m_uiOverloadTimer = 80000;
                }
                else
                    m_uiOverloadTimer -= uiDiff;

                break;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_brundir(Creature* pCreature)
{
    return new boss_brundirAI(pCreature);
}

struct boss_molgeimAI : public ScriptedAI
{
    boss_molgeimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint32 m_uiVisualTimer;
    uint32 m_uiShieldTimer;
    uint32 m_uiRunePowerTimer;
    uint32 m_uiRuneDeathTimer;
    uint32 m_uiRuneSummonTimer;

    void Reset() override
    {
        m_uiPhase               = PHASE_NO_CHARGE;
        m_uiVisualTimer         = 5000;
        m_uiShieldTimer         = 25000;
        m_uiRunePowerTimer      = 15000;
        m_uiRuneSummonTimer     = 10000;
        m_uiRuneDeathTimer      = 30000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // If we are not on the last phase then cast Supercharge and set as unlootable
        if (m_uiPhase != PHASE_CHARGE_TWO)
        {
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCastSpellIfCan(m_creature, SPELL_SUPERCHARGE, CAST_TRIGGERED);
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MOLGEIM, false);
        }
        else
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
            m_creature->CastSpell(m_creature, SPELL_COUNCIL_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }

        DoScriptText(urand(0, 1) ? SAY_MOLGEIM_DEATH_1 : SAY_MOLGEIM_DEATH_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_MOLGEIM_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        m_creature->InterruptNonMeleeSpells(false);
        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_MOLGEIM_SLAY_1 : SAY_MOLGEIM_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_RUNE_OF_SUMMONING)
            pSummoned->CastSpell(pSummoned, SPELL_RUNE_OF_SUMMONING_AURA, true, nullptr, nullptr, m_creature->GetObjectGuid());
        else if (pSummoned->GetEntry() == NPC_RUNE_OF_POWER)
            pSummoned->CastSpell(pSummoned, SPELL_RUNE_OF_POWER_AURA, TRIGGERED_OLD_TRIGGERED);
        else if (pSummoned->GetEntry() == NPC_LIGHTNING_ELEMENTAL)
        {
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_LIGHTNING_ELEMENTAL_PASSIVE : SPELL_LIGHTNING_ELEMENTAL_PASSIVE_H, TRIGGERED_OLD_TRIGGERED);

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // Increase the phase when hit with the supercharge spell by his brothers
        if (pSpell->Id == SPELL_SUPERCHARGE)
        {
            // Not sure if there is a spell for this, so we are doing it here
            m_creature->SetHealth(m_creature->GetMaxHealth());
            ++m_uiPhase;
        }

        if (m_uiPhase == PHASE_CHARGE_TWO)
        {
            // set the instace data to special in order to mark the last phase - this is used to check the achiev criteria
            if (m_pInstance)
                m_pInstance->SetData(TYPE_ASSEMBLY, SPECIAL);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Pre fight visual spell
        if (m_uiVisualTimer)
        {
            if (m_uiVisualTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_POWER_PREFIGHT) == CAST_OK)
                    m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_CHARGE_TWO:

                if (m_uiRuneSummonTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_SUMMONING) == CAST_OK)
                    {
                        DoScriptText(SAY_MOLGEIM_SURGE, m_creature);
                        m_uiRuneSummonTimer = 30000;
                    }
                }
                else
                    m_uiRuneSummonTimer -= uiDiff;

            // no break here; he uses the other spells as well
            case PHASE_CHARGE_ONE:

                if (m_uiRuneDeathTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_RUNE_OF_DEATH : SPELL_RUNE_OF_DEATH_H) == CAST_OK)
                        {
                            DoScriptText(SAY_MOLGEIM_DEATH_RUNE, m_creature);
                            m_uiRuneDeathTimer = 30000;
                        }
                    }
                }
                else
                    m_uiRuneDeathTimer -= uiDiff;

            // no break here; he uses the other spells as well
            case PHASE_NO_CHARGE:

                if (m_uiShieldTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHIELD : SPELL_SHIELD_H) == CAST_OK)
                        m_uiShieldTimer = 40000;
                }
                else
                    m_uiShieldTimer -= uiDiff;

                if (m_uiRunePowerTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_POWER) == CAST_OK)
                        m_uiRunePowerTimer = 45000;
                }
                else
                    m_uiRunePowerTimer -= uiDiff;

                break;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_molgeim(Creature* pCreature)
{
    return new boss_molgeimAI(pCreature);
}

struct boss_steelbreakerAI : public ScriptedAI
{
    boss_steelbreakerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint32 m_uiFusionPunchTimer;
    uint32 m_uiDisruptionTimer;
    uint32 m_uiPowerTimer;

    void Reset() override
    {
        m_uiPhase               = PHASE_NO_CHARGE;
        m_uiFusionPunchTimer    = 15000;
        m_uiDisruptionTimer     = 15000;
        m_uiPowerTimer          = 10000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // If we are not on the last phase then cast Supercharge and set as unlootable
        if (m_uiPhase != PHASE_CHARGE_TWO)
        {
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCastSpellIfCan(m_creature, SPELL_SUPERCHARGE, CAST_TRIGGERED);
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_STEELBREAKER, false);
        }
        else
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
            m_creature->CastSpell(m_creature, SPELL_COUNCIL_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }

        DoScriptText(urand(0, 1) ? SAY_STEEL_DEATH_1 : SAY_STEEL_DEATH_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_STEEL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_STEEL_SLAY_1 : SAY_STEEL_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // Increase the phase when hit with the supercharge spell by his brothers
        if (pSpell->Id == SPELL_SUPERCHARGE)
        {
            // Not sure if there is a spell for this, so we are doing it here
            m_creature->SetHealth(m_creature->GetMaxHealth());
            ++m_uiPhase;
        }

        if (m_uiPhase == PHASE_CHARGE_TWO)
        {
            // Cast electrical charge aura on all players - this will proc when player dies
            DoCastSpellIfCan(m_creature, SPELL_ELECTRICAL_CHARGE, CAST_TRIGGERED);

            // set the instace data to special in order to mark the last phase - this is used to check the achiev criteria
            if (m_pInstance)
                m_pInstance->SetData(TYPE_ASSEMBLY, SPECIAL);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_CHARGE_TWO:

                if (m_uiPowerTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_OVERWHELMING_POWER : SPELL_OVERWHELMING_POWER_H) == CAST_OK)
                    {
                        DoScriptText(SAY_STEEL_OVERWHELM, m_creature);
                        m_uiPowerTimer = m_bIsRegularMode ? 60000 : 35000;
                    }
                }
                else
                    m_uiPowerTimer -= uiDiff;

            // no break here; he uses the other spells as well
            case PHASE_CHARGE_ONE:

                if (m_uiDisruptionTimer < uiDiff)
                {
                    // NOTE: This spell is not cast right: Normally it should be triggered by 64641 in core
                    // Because of the poor target selection in core we'll implement it here with select flag targeting
                    Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_NOT_IN_MELEE_RANGE);

                    if (!pTarget)
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_STATIC_DISRUPTION : SPELL_STATIC_DISRUPTION_H) == CAST_OK)
                        m_uiDisruptionTimer = urand(10000, 15000);
                }
                else
                    m_uiDisruptionTimer -= uiDiff;

            // no break here; he uses the other spells as well
            case PHASE_NO_CHARGE:

                if (m_uiFusionPunchTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_FUSION_PUNCH : SPELL_FUSION_PUNCH_H) == CAST_OK)
                        m_uiFusionPunchTimer = 15000;
                }
                else
                    m_uiFusionPunchTimer -= uiDiff;

                break;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_steelbreaker(Creature* pCreature)
{
    return new boss_steelbreakerAI(pCreature);
}

struct LightningWhirl : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(2, 3));
    }
};

struct LightningWhirlHeroic : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(3, 6));
    }
};

void AddSC_boss_assembly_of_iron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_brundir";
    pNewScript->GetAI = GetAI_boss_brundir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_molgeim";
    pNewScript->GetAI = GetAI_boss_molgeim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_steelbreaker";
    pNewScript->GetAI = GetAI_boss_steelbreaker;
    pNewScript->RegisterSelf();

    RegisterSpellScript<LightningWhirl>("spell_lightning_whirl");
    RegisterSpellScript<LightningWhirlHeroic>("spell_lightning_whirl_heroic");
}
