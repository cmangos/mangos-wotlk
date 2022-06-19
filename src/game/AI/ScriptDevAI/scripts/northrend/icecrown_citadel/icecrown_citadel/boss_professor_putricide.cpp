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
SDName: boss_professor_putricide
SD%Complete: 90%
SDComment: Heroic spells require more research and probably core fix
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631092,
    SAY_AIRLOCK                 = -1631093,
    SAY_PHASE_CHANGE            = -1631094,
    SAY_TRANSFORM_1             = -1631095,
    SAY_TRANSFORM_2             = -1631096,
    SAY_SLAY_1                  = -1631097,
    SAY_SLAY_2                  = -1631098,
    SAY_BERSERK                 = -1631099,
    SAY_DEATH                   = -1631100,

    EMOTE_EXPERIMENT            = -1631206,

    // Rotface encounter yells
    SAY_SLIME_FLOW_1            = -1631074,
    SAY_SLIME_FLOW_2            = -1631075,

    // Spells
    SPELL_BERSERK                   = 47008,

    // Volatile Ooze and Gaz Cloud summon
    SPELL_ORANGE_OOZE_SUMMON        = 71415,                // triggers 71414 to summon 37562
    SPELL_GREEN_OOZE_SUMMON         = 71412,                // triggers 71413 to summon 37697

    // Slime puddle summon
    SPELL_SLIME_PUDDLE_SUMMON       = 70341,                // triggers 70342 to summon 37690
    SPELL_SLIME_PUDDLE_TRIGGER      = 71424,                // triggers 71425 to summon 38234

    SPELL_UNSTABLE_EXPERIMENT       = 70351,                // triggers 71412 or 71415 alternatively

    // Phase 2 spells
    SPELL_CREATE_CONCOCTION         = 71621,                // transform spell
    SPELL_MALLEABLE_GOO             = 72295,                // triggers 70852 on players
    SPELL_CHOKING_GAS_BOMB          = 71255,                // triggers 71273

    // Tear Gas - phase transitions
    SPELL_TEAR_GAS                  = 71617,                // initial tear gas spell on transition start
    SPELL_TEAR_GAS_CANCEL           = 71620,
    SPELL_TEAR_GAS_PERIODIC         = 73170,                // putricide continues to have the periodic aura during transition
    SPELL_VOLATILE_EXPERIMENT       = 72840,                // heroic transition spell; triggers 71412 and 71415 at once

    // Phase 3 transition and spell
    SPELL_CLEANSE_MUTATION          = 71693,                // cancel abomination
    SPELL_GUZZLE_POTIONS            = 71893,                // triggers 71704 and 71621
    SPELL_MUTATED_PLAGUE            = 72451,

    // heroic spells
    SPELL_UNBOUND_PLAGUE            = 70911,
    SPELL_OOZE_VARIABLE             = 70352,                // ToDo: research how this should work
    SPELL_OOZE_VARIABLE_OOZE        = 74118,
    SPELL_GAS_VARIABLE              = 70353,                // ToDo: research how this should work
    SPELL_GAS_VARIABLE_GAS          = 74119,

    SPELL_OOZE_TANK_PROTECTION      = 71770,                // ToDo: research how this should work

    // spells used for other encounters
    SPELL_OOZE_FLOOD_TRIGGER        = 69795,                // triggers 69782 on top pipes targets - 37013
    SPELL_OOZE_FLOOD                = 69782,                // triggers 69783 on the closest lower pipe target - 37013

    // Summoned spells
    // growing ooze puddle
    SPELL_SLIME_PUDDLE_DAMAGE       = 70343,
    SPELL_GROW_STACKER              = 70345,                // triggers 70347
    //SPELL_GROW_STACKER_GROW_AURA  = 70347,
    SPELL_EAT_OOZE                  = 70360,

    // choking gas bomb
    SPELL_CHOKING_GAS_PERIODIC      = 71259,
    SPELL_CHOKING_GAS_EXPLOSION_PER = 71280,

    // Volatile Ooze
    SPELL_OOZE_ADHESIVE             = 70447,                // Cast on player and follow the player to errupt
    SPELL_OOZE_ERUPTION             = 70492,

    // Gas cloud
    SPELL_GASEOUS_BLOAT             = 70672,                // Chase players and expunge gas
    SPELL_EXPUNGED_GAS              = 70701,
    SPELL_GASEOUS_BLOAT_VISUAL      = 70215,

    // Mutated Abomination (scripted in ACID and kept here only for reference)
    //SPELL_TRANSFORMATION_NAME       = 72402,
    //SPELL_TRANSFORMATION_WHISPER    = 70438,              // use unk
    //SPELL_TRANSFORMATION_DAMAGE     = 70405,
    //SPELL_POWER_DRAIN               = 70385,

    // npcs
    NPC_GROWING_OOZE_PUDDLE_TRIGGER = 38234,
    NPC_GROWING_OOZE_PUDDLE         = 37690,
    NPC_GAS_CLOUD                   = 37562,
    NPC_VOLATILE_OOZE               = 37697,
    NPC_CHOKING_GAS_BOMB            = 38159,
};

enum Phase
{
    PHASE_ONE               = 1,
    PHASE_RUNNING_ONE       = 2,
    PHASE_TRANSITION_ONE    = 3,
    PHASE_TWO               = 4,
    PHASE_RUNNING_TWO       = 5,
    PHASE_TRANSITION_TWO    = 6,
    PHASE_THREE             = 7
};

enum Waypoint
{
    POINT_PUTRICIDE_SPAWN   = 1,
    POINT_FESTERGUT_BALCONY = 101,
    POINT_ROTFACE_BALCONY   = 102,
};

/*######
## boss_professor_putricide
######*/

struct boss_professor_putricideAI : public ScriptedAI
{
    boss_professor_putricideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    bool m_bIsOrange;

    uint32 m_uiPhase;

    uint32 m_uiTransitionTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiPuddleTimer;
    uint32 m_uiUnstableExperimentTimer;
    uint32 m_uiUnboundPlagueTimer;
    uint32 m_uiChokingGasBombTimer;
    uint32 m_uiMalleableGooTimer;

    void Reset() override
    {
        m_bIsOrange                 = urand(0, 1) != 0;
        m_uiPhase                   = PHASE_ONE;
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;
        m_uiTransitionTimer         = 0;
        m_uiPuddleTimer             = 10000;
        m_uiUnstableExperimentTimer = 30000;
        m_uiUnboundPlagueTimer      = 10000;
        m_uiChokingGasBombTimer     = 35000;
        m_uiMalleableGooTimer       = 25000;

        // set or remove not selectable flag depending on Festergut and Rotface
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ROTFACE) != DONE || m_pInstance->GetData(TYPE_FESTERGUT) != DONE)
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
            else
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, FAIL);

            if (Creature* pTentacle = m_pInstance->GetSingleCreatureFromStorage(NPC_OOZE_TENTACLE_STALKER))
                pTentacle->RemoveAllAurasOnEvade();
            if (Creature* pTentacle = m_pInstance->GetSingleCreatureFromStorage(NPC_SLIMY_TENTACLE_STALKER))
                pTentacle->RemoveAllAurasOnEvade();
        }
    }

    void AttackStart(Unit* pWho) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        // handle phase transition
        if (uiData == POINT_PUTRICIDE_SPAWN)
        {
            if (!m_pInstance)
                return;

            DoCastSpellIfCan(m_creature, (m_uiPhase == PHASE_RUNNING_ONE ? SPELL_CREATE_CONCOCTION : SPELL_GUZZLE_POTIONS));

            if (m_pInstance->IsHeroicDifficulty())
            {
                DoScriptText(SAY_PHASE_CHANGE, m_creature);
                m_uiTransitionTimer = 30000;
            }
            else
                m_uiTransitionTimer = 15000;

            m_uiPhase = (m_uiPhase == PHASE_RUNNING_ONE ? PHASE_TRANSITION_ONE : PHASE_TRANSITION_TWO);           // waiting for entering next phase
        }
        else if (uiData == POINT_FESTERGUT_BALCONY)
            m_creature->SetFacingTo(3.316f);
        else if (uiData == POINT_ROTFACE_BALCONY)
            m_creature->SetFacingTo(5.822f);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_GROWING_OOZE_PUDDLE_TRIGGER:
                DoCastSpellIfCan(pSummoned, SPELL_SLIME_PUDDLE_SUMMON, CAST_TRIGGERED);
                pSummoned->ForcedDespawn(10000);
                break;
            case NPC_GROWING_OOZE_PUDDLE:
                pSummoned->CastSpell(pSummoned, SPELL_GROW_STACKER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_SLIME_PUDDLE_DAMAGE, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_CHOKING_GAS_BOMB:
                pSummoned->CastSpell(pSummoned, SPELL_CHOKING_GAS_PERIODIC, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_CHOKING_GAS_EXPLOSION_PER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->ForcedDespawn(15000);
                break;
            case NPC_GAS_CLOUD:
                pSummoned->CastSpell(pSummoned, SPELL_GASEOUS_BLOAT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // no break;
            case NPC_VOLATILE_OOZE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                pSummoned->SetInCombatWithZone();
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (!m_pInstance)
            return;

        // Unstable experiment
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetScientistStalkerGuid(m_bIsOrange)))
                DoCastSpellIfCan(pStalker, m_bIsOrange ? SPELL_ORANGE_OOZE_SUMMON : SPELL_GREEN_OOZE_SUMMON, CAST_TRIGGERED);

            m_bIsOrange = !m_bIsOrange;
        }
        // Volatile Experiment
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetScientistStalkerGuid(true)))
                DoCastSpellIfCan(pStalker, SPELL_ORANGE_OOZE_SUMMON, CAST_TRIGGERED);
            if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_pInstance->GetScientistStalkerGuid(false)))
                DoCastSpellIfCan(pStalker, SPELL_GREEN_OOZE_SUMMON, CAST_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_pInstance)
            return;

        // Enrage
        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiEnrageTimer = 0;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_TWO:
            case PHASE_THREE:
            {
                // Choking Gas
                if (m_uiChokingGasBombTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CHOKING_GAS_BOMB) == CAST_OK)
                        m_uiChokingGasBombTimer = 35000;
                }
                else
                    m_uiChokingGasBombTimer -= uiDiff;

                // Malleable Goo
                if (m_uiMalleableGooTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MALLEABLE_GOO) == CAST_OK)
                        m_uiMalleableGooTimer = 25000;
                }
                else
                    m_uiMalleableGooTimer -= uiDiff;

                if (m_uiPhase == PHASE_THREE)
                    break;
                // else no break;
            }
            case PHASE_ONE:
            {
                if (m_uiTransitionTimer)
                {
                    if (m_uiTransitionTimer <= uiDiff)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_PERIODIC, CAST_TRIGGERED);

                        float fX, fY, fZ;
                        SetCombatMovement(false);

                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetRespawnCoord(fX, fY, fZ);
                        m_creature->GetMotionMaster()->MovePoint(POINT_PUTRICIDE_SPAWN, fX, fY, fZ);

                        m_uiPhase = (m_uiPhase == PHASE_ONE ? PHASE_RUNNING_ONE : PHASE_RUNNING_TWO);
                        m_uiTransitionTimer = 0;
                    }
                    else
                        m_uiTransitionTimer -= uiDiff;

                    // Preparing for phase transition
                    return;
                }

                // Unbound Plague
                if (m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiUnboundPlagueTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                                m_uiUnboundPlagueTimer = 70000;
                        }
                    }
                    else
                        m_uiUnboundPlagueTimer -= uiDiff;
                }

                // Unstable Experiment
                if (m_uiUnstableExperimentTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
                    {
                        DoScriptText(EMOTE_EXPERIMENT, m_creature);
                        m_uiUnstableExperimentTimer = 37000;
                    }
                }
                else
                    m_uiUnstableExperimentTimer -= uiDiff;

                // health check
                if (m_creature->GetHealthPercent() <= (m_uiPhase == PHASE_ONE ? 80.0f : 35.0f))
                {
                    uint32 spellId = (m_pInstance->IsHeroicDifficulty() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS);

                    if (DoCastSpellIfCan(m_creature, spellId, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        m_uiTransitionTimer = 2500;
                        return;
                    }
                }

                break;
            }
            // Transition phases
            case PHASE_TRANSITION_ONE:
            case PHASE_TRANSITION_TWO:
            {
                if (m_uiTransitionTimer < uiDiff)
                {
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

                    DoScriptText((m_uiPhase == PHASE_TRANSITION_ONE ? SAY_TRANSFORM_1 : SAY_TRANSFORM_2), m_creature);

                    // in heroic it changes form at the end of the transition
                    if (!m_pInstance->IsHeroicDifficulty())
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_CANCEL, CAST_TRIGGERED);

                    m_uiPhase = (m_uiPhase == PHASE_TRANSITION_ONE ? PHASE_TWO : PHASE_THREE);
                    m_uiTransitionTimer = 0;
                }
                else
                    m_uiTransitionTimer -= uiDiff;

                return;
            }
            // Movement transition phases
            case PHASE_RUNNING_ONE:
            case PHASE_RUNNING_TWO:
            {
                // wait for arriving at the table (during phase transition)
                return;
            }
        }

        // Slime Puddle (all phases, except for transition
        if (m_uiPuddleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SLIME_PUDDLE_TRIGGER) == CAST_OK)
                m_uiPuddleTimer = 35000;
        }
        else
            m_uiPuddleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_professor_putricide(Creature* pCreature)
{
    return new boss_professor_putricideAI(pCreature);
}

bool EffectScriptEffectCreature_npc_putricide(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (pCreatureTarget->GetEntry() != NPC_PROFESSOR_PUTRICIDE)
        return false;

    // Ooze flood trigger - used in Rotface encounter
    if (uiSpellId == SPELL_OOZE_FLOOD_TRIGGER && uiEffIndex == EFFECT_INDEX_0)
    {
        instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pCreatureTarget->GetInstanceData();
        if (!pInstance)
            return false;

        // Defind the target as one of the stalkers from the top of the taps
        GuidList lStalkersGuidList;
        pInstance->GetRotfaceStalkersList(lStalkersGuidList);

        std::vector<Creature*> vTapStalkers;
        vTapStalkers.reserve(lStalkersGuidList.size());

        for (GuidList::const_iterator itr = lStalkersGuidList.begin(); itr != lStalkersGuidList.end(); ++itr)
        {
            if (Creature* pStalker = pCreatureTarget->GetMap()->GetCreature(*itr))
                vTapStalkers.push_back(pStalker);
        }

        if (vTapStalkers.empty())
        {
            script_error_log("Instance Icecrown Citadel: ERROR Failed to properly find creature %u for Ooze Flood event.", NPC_PUDDLE_STALKER);
            return false;
        }

        // pick random target of the tap stalkers
        Creature* pTarget = vTapStalkers[urand(0, vTapStalkers.size() - 1)];
        if (!pTarget)
            return false;

        // get the nearest twin tap stalker
        Creature* pNearTarget = nullptr;
        CreatureList lTargetsInRange;
        GetCreatureListWithEntryInGrid(lTargetsInRange, pTarget, pTarget->GetEntry(), 30.0f);

        if (lTargetsInRange.empty())
            return false;

        // find only the nearest tap trigger
        for (CreatureList::const_iterator itr = lTargetsInRange.begin(); itr != lTargetsInRange.end(); ++itr)
        {
            if ((*itr)->GetPositionZ() > 370.0f)
                pNearTarget = *itr;
        }

        if (!pNearTarget)
            return false;

        // cast the triggered spell on each target
        pCreatureTarget->CastSpell(pTarget, GetSpellStore()->LookupEntry<SpellEntry>(uiSpellId)->CalculateSimpleValue(uiEffIndex), TRIGGERED_OLD_TRIGGERED);
        pCreatureTarget->CastSpell(pNearTarget, GetSpellStore()->LookupEntry<SpellEntry>(uiSpellId)->CalculateSimpleValue(uiEffIndex), TRIGGERED_OLD_TRIGGERED);
        DoScriptText(urand(0, 1) ? SAY_SLIME_FLOW_1 : SAY_SLIME_FLOW_2, pCreatureTarget);

        return true;
    }
    if (uiSpellId == SPELL_UNSTABLE_EXPERIMENT && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

bool EffectDummyCreature_npc_putricide(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (pCreatureTarget->GetEntry() != NPC_PROFESSOR_PUTRICIDE)
        return false;

    if (uiSpellId == SPELL_VOLATILE_EXPERIMENT && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## npc_volatile_ooze_icc
######*/

struct npc_volatile_ooze_iccAI : public ScriptedAI
{
    npc_volatile_ooze_iccAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAdhesiveTimer;

    ObjectGuid m_targetGuid;

    void Reset() override
    {
        m_uiAdhesiveTimer = urand(2000, 4000);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiAdhesiveTimer && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, ATTACK_DISTANCE) && pWho->GetObjectGuid() == m_targetGuid)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_OOZE_ERUPTION) == CAST_OK)
            {
                m_uiAdhesiveTimer = urand(3000, 5000);
                m_targetGuid.Clear();
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAdhesiveTimer)
        {
            if (m_uiAdhesiveTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_OOZE_ADHESIVE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_OOZE_ADHESIVE) == CAST_OK)
                    {
                        m_creature->GetMotionMaster()->MoveFollow(pTarget, ATTACK_DISTANCE * 0.5f, m_creature->GetAngle(pTarget));
                        m_targetGuid = pTarget->GetObjectGuid();
                        m_uiAdhesiveTimer = 0;
                    }
                }
            }
            else
                m_uiAdhesiveTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_volatile_ooze_icc(Creature* pCreature)
{
    return new npc_volatile_ooze_iccAI(pCreature);
}

/*######
## npc_gas_cloud_icc
######*/

struct npc_gas_cloud_iccAI : public ScriptedAI
{
    npc_gas_cloud_iccAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiGasBloatTimer;

    ObjectGuid m_targetGuid;

    void Reset() override
    {
        m_uiGasBloatTimer = urand(2000, 4000);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiGasBloatTimer && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, ATTACK_DISTANCE) && pWho->GetObjectGuid() == m_targetGuid)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EXPUNGED_GAS) == CAST_OK)
            {
                m_uiGasBloatTimer = urand(3000, 5000);
                m_targetGuid.Clear();
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiGasBloatTimer)
        {
            if (m_uiGasBloatTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_OOZE_ADHESIVE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_GASEOUS_BLOAT) == CAST_OK)
                    {
                        m_creature->GetMotionMaster()->MoveFollow(pTarget, ATTACK_DISTANCE * 0.5f, m_creature->GetAngle(pTarget));
                        m_targetGuid = pTarget->GetObjectGuid();
                        m_uiGasBloatTimer = 0;
                    }
                }
            }
            else
                m_uiGasBloatTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_gas_cloud_icc(Creature* pCreature)
{
    return new npc_gas_cloud_iccAI(pCreature);
}

/*######
## npc_growing_ooze_puddle
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_growing_ooze_puddleAI : public Scripted_NoMovementAI
{
    npc_growing_ooze_puddleAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_growing_ooze_puddle(Creature* pCreature)
{
    return new npc_growing_ooze_puddleAI(pCreature);
}

bool EffectScriptEffectCreature_spell_eat_ooze(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (pCreatureTarget->GetEntry() != NPC_GROWING_OOZE_PUDDLE)
        return false;

    // Ooze flood trigger - used in Rotface encounter
    if (uiSpellId == SPELL_EAT_OOZE && uiEffIndex == EFFECT_INDEX_0)
    {
        instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pCreatureTarget->GetInstanceData();
        if (!pInstance)
            return false;

        // ToDo: research if the amount of stacks removal is correct
        pCreatureTarget->RemoveAuraHolderFromStack(GetSpellStore()->LookupEntry<SpellEntry>(uiSpellId)->CalculateSimpleValue(uiEffIndex), 3);

        // despawn if we don't have the grow aura anymore
        if (!pCreatureTarget->HasAura(GetSpellStore()->LookupEntry<SpellEntry>(uiSpellId)->CalculateSimpleValue(uiEffIndex)))
        {
            pCreatureTarget->RemoveAllAurasOnEvade();
            pCreatureTarget->ForcedDespawn(5000);
        }

        pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NAUSEA, false);
        return true;
    }

    return false;
}

/*######
## npc_choking_gas_bomb
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_choking_gas_bombAI : public Scripted_NoMovementAI
{
    npc_choking_gas_bombAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_choking_gas_bomb(Creature* pCreature)
{
    return new npc_choking_gas_bombAI(pCreature);
}

/*######
## npc_puddle_stalker
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_puddle_stalkerAI : public Scripted_NoMovementAI
{
    npc_puddle_stalkerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_puddle_stalker(Creature* pCreature)
{
    return new npc_puddle_stalkerAI(pCreature);
}

bool EffectScriptEffectCreature_spell_ooze_flood(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_OOZE_FLOOD && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_PUDDLE_STALKER)
    {
        // Set target manually to hit exactly the stalker below the tap
        if (Creature* pStalker = GetClosestCreatureWithEntry(pCreatureTarget, pCreatureTarget->GetEntry(), 20.0f))
            pCreatureTarget->CastSpell(pStalker, GetSpellStore()->LookupEntry<SpellEntry>(uiSpellId)->CalculateSimpleValue(uiEffIndex), TRIGGERED_OLD_TRIGGERED);

        return true;
    }

    return false;
}

void AddSC_boss_professor_putricide()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_professor_putricide";
    pNewScript->GetAI = &GetAI_boss_professor_putricide;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_putricide;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_npc_putricide;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_volatile_ooze_icc";
    pNewScript->GetAI = GetAI_npc_volatile_ooze_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gas_cloud_icc";
    pNewScript->GetAI = GetAI_npc_gas_cloud_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_growing_ooze_puddle";
    pNewScript->GetAI = GetAI_npc_growing_ooze_puddle;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_eat_ooze;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_choking_gas_bomb";
    pNewScript->GetAI = GetAI_npc_choking_gas_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_puddle_stalker";
    pNewScript->GetAI = GetAI_npc_puddle_stalker;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_ooze_flood;
    pNewScript->RegisterSelf();
}
