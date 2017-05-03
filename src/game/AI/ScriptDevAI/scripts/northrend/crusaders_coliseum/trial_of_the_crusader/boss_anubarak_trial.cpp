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
SDName: boss_anubarak_trial
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "trial_of_the_crusader.h"

enum
{
    SAY_INTRO                           = -1649038,
    SAY_AGGRO                           = -1649064,
    SAY_SLAY_1                          = -1649065,
    SAY_SLAY_2                          = -1649066,
    SAY_DEATH                           = -1649067,
    SAY_BERSERK                         = -1649068,
    SAY_SUBMERGE                        = -1649069,
    SAY_LEECHING_SWARM                  = -1649070,

    EMOTE_BURROW                        = -1649071,
    EMOTE_PURSUE                        = -1649072,
    EMOTE_EMERGE                        = -1649073,
    EMOTE_LEECHING_SWARM                = -1649074,

    // Anub'arak
    SPELL_FREEZING_SLASH                = 66012,
    SPELL_PENETRATING_COLD              = 66013,
    SPELL_SUMMON_NERUBIAN_BURROWER      = 66332,
    SPELL_SUMMON_SCARAB                 = 66339,
    SPELL_SUBMERGE                      = 65981,
    SPELL_EMERGE                        = 65982,
    SPELL_TELEPORT_TO_SPIKE             = 66170,            // used when the underground phase ends
    SPELL_CLEAR_ALL_DEBUFFS             = 34098,
    SPELL_SUMMON_SPIKES                 = 66169,
    SPELL_LEECHING_SWARM                = 66118,
    SPELL_BERSERK                       = 26662,

    SPELL_SCARAB_ACHIEV_10              = 68186,            // used for achiev 3800
    SPELL_SCARAB_ACHIEV_25              = 68515,            // used for achiev 3816

    // Pursuing Spikes
    SPELL_PURSUING_SPIKES_FAIL          = 66181,
    SPELL_PURSUING_SPIKES_DUMMY         = 67470,            // target selection spell
    SPELL_PURSUING_SPIKES_SPEED1        = 65920,
    // SPELL_PURSUING_SPIKES_GROUND      = 65921,           // included in creature_template_addon
    SPELL_PURSUING_SPIKES_SPEED2        = 65922,
    SPELL_PURSUING_SPIKES_SPEED3        = 65923,
    SPELL_MARK                          = 67574,

    // Frostsphere
    SPELL_PERMAFROST_VISUAL             = 65882,            // triggers 65872
    SPELL_PERMAFROST_DUMMY              = 65872,            // dummy spell which handles the spike fail event
    SPELL_PERMAFROST_TRANSFORM          = 66185,
    SPELL_PERMAFROST_SLOW               = 66193,            // slow spell
    SPELL_FROSTSPHERE_VISUAL            = 67539,

    POINT_GROUND                        = 0,

    // npcs
    NPC_SCARAB                          = 34605,
    NPC_FROSTSPHERE                     = 34606,
    NPC_NERUBIAN_BURROWER               = 34607,
    NPC_ANUBARAK_SPIKE                  = 34660,
    NPC_BURROW                          = 34862,

    MAX_FROSTSPHERES                    = 6,
    MAX_BURROWS                         = 4
};

enum Phases
{
    PHASE_GROUND         = 0,
    PHASE_UNDERGROUND    = 1,
    PHASE_LEECHING_SWARM = 2,
    PHASE_SUBMERGING     = 3,                               // virtual use only while casting SPELL_SUBMERGE (triggered by script!)
};

enum PursuingSpikesPhases
{
    PHASE_NO_MOVEMENT   = 0,
    PHASE_IMPALE_NORMAL = 1,
    PHASE_IMPALE_MIDDLE = 2,
    PHASE_IMPALE_FAST   = 3
};

static const float aFrostSphereSpawnPositions[MAX_FROSTSPHERES][3] =
{
    { 701.4270f, 126.4739f, 158.0205f },
    { 712.5711f, 160.9948f, 158.4367f },
    { 736.0243f, 113.4201f, 158.0225f },
    { 747.9201f, 155.0920f, 158.0613f },
    { 769.6285f, 121.1024f, 158.0504f },
    { 779.8038f, 150.658f,  158.1426f }
};

static const float aBurrowSpawnPositions[MAX_BURROWS][4] =
{
    { 735.4028f, 75.35764f, 142.2023f, 2.094395f  },
    { 692.9202f, 184.809f,  142.2026f, 5.358161f  },
    { 688.2066f, 102.8472f, 142.2023f, 0.6457718f },
    { 740.5452f, 189.1129f, 142.1972f, 3.752458f  }
};

/*######
## boss_anubarak_trial
######*/

struct boss_anubarak_trialAI : public ScriptedAI
{
    boss_anubarak_trialAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        m_bDidIntroYell = false;
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    Phases m_Phase;

    uint32 m_PhaseSwitchTimer;
    uint32 m_uiFreezingSlashTimer;
    uint32 m_uiPenetratingColdTimer;
    uint32 m_uiBurrowerSummonTimer;
    uint32 m_uiBerserkTimer;
    bool m_bDidIntroYell;

    ObjectGuid m_PursuingSpikesGuid;
    GuidVector m_vSpheresGuidVector;

    void Reset() override
    {
        m_Phase                  = PHASE_GROUND;
        m_PhaseSwitchTimer       = 80000;
        m_uiFreezingSlashTimer   = 20000;
        m_uiPenetratingColdTimer = urand(15000, 25000);
        m_uiBurrowerSummonTimer  = 10000;
        m_uiBerserkTimer         = 10 * MINUTE * IN_MILLISECONDS;

        m_vSpheresGuidVector.clear();
        m_vSpheresGuidVector.resize(MAX_FROSTSPHERES, ObjectGuid());
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, DONE);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bDidIntroYell && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() &&
                !m_creature->IsInEvadeMode() && pWho->IsWithinDistInMap(m_creature, 100) && pWho->IsWithinLOSInMap(m_creature))
        {
            DoScriptText(SAY_INTRO, m_creature);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
            DoCastSpellIfCan(m_creature, SPELL_EMERGE);

            m_bDidIntroYell = true;
            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        // Summon the spheres on random points
        for (uint8 i = 0; i < MAX_FROSTSPHERES; ++i)
        {
            if (Creature* pTemp = m_creature->SummonCreature(NPC_FROSTSPHERE, aFrostSphereSpawnPositions[i][0], aFrostSphereSpawnPositions[i][1], aFrostSphereSpawnPositions[i][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                m_vSpheresGuidVector[i] = pTemp->GetObjectGuid();
        }

        // It's not clear if these should be spawned by DB or summoned
        for (uint8 i = 0; i < MAX_BURROWS; ++i)
            m_creature->SummonCreature(NPC_BURROW, aBurrowSpawnPositions[i][0], aBurrowSpawnPositions[i][1], aBurrowSpawnPositions[i][2], aBurrowSpawnPositions[i][3], TEMPSUMMON_DEAD_DESPAWN, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText((urand(0, 1)) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SUBMERGE)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // Extra check here, because AnubArak must be submerged by default
            if (m_Phase != PHASE_SUBMERGING)
                return;

            m_Phase = PHASE_UNDERGROUND;

            // Refresh spheres only on normal difficulty
            if (m_pInstance && !m_pInstance->IsHeroicDifficulty())
                DoRefreshSpheres();

            DoCastSpellIfCan(m_creature, SPELL_CLEAR_ALL_DEBUFFS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPIKES, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCARAB, CAST_TRIGGERED);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ANUBARAK_SPIKE:
                m_PursuingSpikesGuid = pSummoned->GetObjectGuid();
                // no break here
            case NPC_NERUBIAN_BURROWER:
            case NPC_SCARAB:
                pSummoned->AI()->AttackStart(m_creature->getVictim());
                break;
        }
    }

    // Wrapper to refresh frost spheres - it's not very clear how ofter should this happen
    void DoRefreshSpheres()
    {
        for (uint8 i = 0; i < MAX_FROSTSPHERES; ++i)
        {
            // If the sphere is alive and hasn't transfomed to permafrost yet summon a new one
            Creature* pTemp = m_creature->GetMap()->GetCreature(m_vSpheresGuidVector[i]);
            if (pTemp && !pTemp->HasAura(SPELL_PERMAFROST_TRANSFORM))
                continue;

            // Summon a new frost sphere instead of the killed one
            if (Creature* pTemp = m_creature->SummonCreature(NPC_FROSTSPHERE, aFrostSphereSpawnPositions[i][0], aFrostSphereSpawnPositions[i][1], aFrostSphereSpawnPositions[i][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                m_vSpheresGuidVector[i] = pTemp->GetObjectGuid();
        }
    }

    // Wrapper to despawn the Spikes
    void DoDespawnPursuingSpikes()
    {
        if (Creature* pPursuingSpikes = m_creature->GetMap()->GetCreature(m_PursuingSpikesGuid))
            pPursuingSpikes->ForcedDespawn();

        m_PursuingSpikesGuid.Clear();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_Phase)
        {
            case PHASE_GROUND:

                // Switch to underground phase on timer
                if (m_PhaseSwitchTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE) == CAST_OK)
                    {
                        DoScriptText(SAY_SUBMERGE, m_creature);
                        DoScriptText(EMOTE_BURROW, m_creature);
                        m_PhaseSwitchTimer = 63000;
                        m_Phase = PHASE_SUBMERGING;
                        return;
                    }
                }
                else
                    m_PhaseSwitchTimer -= uiDiff;

                // Switch to phase 3 when below 30%
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_LEECHING_SWARM) == CAST_OK)
                    {
                        DoScriptText(SAY_LEECHING_SWARM, m_creature);
                        DoScriptText(EMOTE_LEECHING_SWARM, m_creature);
                        m_Phase = PHASE_LEECHING_SWARM;
                    }
                }

                // No break - the spells are used in both phase 1 and 3
            case PHASE_LEECHING_SWARM:

                if (m_uiFreezingSlashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FREEZING_SLASH) == CAST_OK)
                        m_uiFreezingSlashTimer = 20000;
                }
                else
                    m_uiFreezingSlashTimer -= uiDiff;

                if (m_uiPenetratingColdTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PENETRATING_COLD) == CAST_OK)
                        m_uiPenetratingColdTimer = 15000;
                }
                else
                    m_uiPenetratingColdTimer -= uiDiff;

                // The Borrowers are summoned in Ground phase only on normal mode or during Ground and Swarm phase on heroic mode
                if (m_Phase == PHASE_GROUND || (m_pInstance && m_pInstance->IsHeroicDifficulty()))
                {
                    if (m_uiBurrowerSummonTimer < uiDiff)
                    {
                        // The number of targets is handled in core, based on difficulty
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_NERUBIAN_BURROWER) == CAST_OK)
                            m_uiBurrowerSummonTimer = 45000;
                    }
                    else
                        m_uiBurrowerSummonTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;

            case PHASE_UNDERGROUND:

                // Underground phase is finished
                if (m_PhaseSwitchTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_TELEPORT_TO_SPIKE, CAST_TRIGGERED);
                    DoScriptText(EMOTE_EMERGE, m_creature);
                    DoDespawnPursuingSpikes();

                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    // Refresh spheres only on normal difficulty
                    if (m_pInstance && !m_pInstance->IsHeroicDifficulty())
                        DoRefreshSpheres();

                    m_PhaseSwitchTimer = 80000;
                    m_Phase = PHASE_GROUND;
                }
                else
                    m_PhaseSwitchTimer -= uiDiff;

                break;
            case PHASE_SUBMERGING:                          // Do nothing, but continue berserk timer
                break;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (m_Phase != PHASE_UNDERGROUND)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    {
                        DoScriptText(SAY_BERSERK, m_creature);
                        m_uiBerserkTimer = 0;
                    }
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_anubarak_trial(Creature* pCreature)
{
    return new boss_anubarak_trialAI(pCreature);
}

/*######
## npc_anubarak_trial_spike
######*/

struct npc_anubarak_trial_spikeAI : public ScriptedAI
{
    npc_anubarak_trial_spikeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    PursuingSpikesPhases m_Phase;
    uint32 m_PhaseSwitchTimer;

    void Reset() override
    {
        m_Phase = PHASE_NO_MOVEMENT;
        m_PhaseSwitchTimer = 5000;

        SetCombatMovement(false);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_PURSUING_SPIKES_DUMMY && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(EMOTE_PURSUE, m_creature, pTarget);
            DoCastSpellIfCan(pTarget, SPELL_MARK, CAST_TRIGGERED);
            DoStartMovement(pTarget);
        }
    }

    // Handle permafrost hit from dummy spell
    void PermafrostHit(Creature* pPermafrost)
    {
        // To prevent more than one call
        if (m_Phase == PHASE_NO_MOVEMENT)
            return;

        // Remove the speed auras
        switch (m_Phase)
        {
            case PHASE_IMPALE_NORMAL:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED1);
                break;
            case PHASE_IMPALE_MIDDLE:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED2);
                break;
            case PHASE_IMPALE_FAST:
                m_creature->RemoveAurasDueToSpell(SPELL_PURSUING_SPIKES_SPEED3);
                break;
            default:
                break;
        }

        // Set Spike fail animation and despawn
        DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_FAIL, CAST_TRIGGERED);

        if (pPermafrost)
            pPermafrost->ForcedDespawn(2000);

        // After the spikes hit the icy surface they can't move for about ~5 seconds
        m_Phase = PHASE_NO_MOVEMENT;
        m_PhaseSwitchTimer = 5000;
        DoResetThreat();

        SetCombatMovement(false);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_PhaseSwitchTimer)
        {
            if (m_PhaseSwitchTimer <= uiDiff)
            {
                switch (m_Phase)
                {
                    case PHASE_NO_MOVEMENT:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED1) == CAST_OK)
                        {
                            DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_DUMMY, CAST_TRIGGERED);

                            m_Phase = PHASE_IMPALE_NORMAL;
                            m_PhaseSwitchTimer = 7000;
                        }
                        break;
                    case PHASE_IMPALE_NORMAL:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED2) == CAST_OK)
                        {
                            m_Phase = PHASE_IMPALE_MIDDLE;
                            m_PhaseSwitchTimer = 7000;
                        }
                        break;
                    case PHASE_IMPALE_MIDDLE:
                        if (DoCastSpellIfCan(m_creature, SPELL_PURSUING_SPIKES_SPEED3) == CAST_OK)
                        {
                            m_Phase = PHASE_IMPALE_FAST;
                            m_PhaseSwitchTimer = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
            else
                m_PhaseSwitchTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_anubarak_trial_spike(Creature* pCreature)
{
    return new npc_anubarak_trial_spikeAI(pCreature);
}

bool EffectDummyCreature_spell_dummy_permafrost(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_PERMAFROST_DUMMY && uiEffIndex == EFFECT_INDEX_0)
    {
        if (npc_anubarak_trial_spikeAI* pSpikeAI = dynamic_cast<npc_anubarak_trial_spikeAI*>(pCreatureTarget->AI()))
            pSpikeAI->PermafrostHit((Creature*)pCaster);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_anubarak_trial_frostsphere
######*/

struct npc_anubarak_trial_frostsphereAI : public Scripted_NoMovementAI
{
    npc_anubarak_trial_frostsphereAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    bool m_bPermafrost;

    void Reset() override
    {
        m_bPermafrost = false;

        m_creature->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damagetype*/) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Set fake death in order to apply permafrost
        uiDamage = 0;

        if (m_bPermafrost)
            return;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(0);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTargetGuid(ObjectGuid());

        // Set proper Z position
        m_creature->SetWalk(false);
        float fZ = pDoneBy->GetPositionZ();
        MaNGOS::NormalizeMapCoord(fZ);

        // Note: This should be fall movement
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), fZ);
        m_bPermafrost = true;
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_VISUAL, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_TRANSFORM, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST_SLOW, CAST_TRIGGERED);
    }
};

CreatureAI* GetAI_npc_anubarak_trial_frostsphere(Creature* pCreature)
{
    return new npc_anubarak_trial_frostsphereAI(pCreature);
}

/*######
## npc_nerubian_borrow
######*/

// TODO Remove this 'script' when combat movement can be proper prevented from core-side
struct npc_nerubian_borrowAI : public Scripted_NoMovementAI
{
    npc_nerubian_borrowAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_nerubian_borrow(Creature* pCreature)
{
    return new npc_nerubian_borrowAI(pCreature);
}

void AddSC_boss_anubarak_trial()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_anubarak_trial";
    pNewScript->GetAI = &GetAI_boss_anubarak_trial;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_anubarak_spike";
    pNewScript->GetAI = &GetAI_npc_anubarak_trial_spike;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_dummy_permafrost;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frost_sphere";
    pNewScript->GetAI = &GetAI_npc_anubarak_trial_frostsphere;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nerubian_borrow";
    pNewScript->GetAI = &GetAI_npc_nerubian_borrow;
    pNewScript->RegisterSelf();
}
