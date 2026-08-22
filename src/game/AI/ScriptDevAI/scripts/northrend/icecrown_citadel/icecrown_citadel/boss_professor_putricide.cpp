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
SDComment: Native phase flow, heroic variables, Unbound Plague and Mutated Plague implemented.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = 37838,
    SAY_PHASE_CHANGE            = 37846,
    SAY_TRANSFORM_1             = 37844,
    SAY_TRANSFORM_2             = 37845,
    SAY_SLAY_1                  = 33033,
    SAY_SLAY_2                  = 37840,
    SAY_BERSERK                 = 37842,
    SAY_DEATH                   = 37841,

    EMOTE_EXPERIMENT            = 38500,

    // Rotface encounter yells
    SAY_SLIME_FLOW_1            = 38120,
    SAY_SLIME_FLOW_2            = 37049,

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

struct boss_professor_putricideAI : public CombatAI
{
    boss_professor_putricideAI(Creature* creature) : CombatAI(creature, 0)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        Reset();
    }

    instance_icecrown_citadel* m_instance;

    bool m_isOrange;

    uint32 m_phase;

    uint32 m_transitionTimer;
    uint32 m_enrageTimer;
    uint32 m_puddleTimer;
    uint32 m_unstableExperimentTimer;
    uint32 m_unboundPlagueTimer;
    uint32 m_mutatedPlagueTimer;
    uint32 m_chokingGasBombTimer;
    uint32 m_malleableGooTimer;

    void Reset() override
    {
        CombatAI::Reset();
        m_isOrange           = urand(0, 1) != 0;
        m_phase              = PHASE_ONE;
        m_enrageTimer        = 10 * MINUTE * IN_MILLISECONDS;
        m_transitionTimer    = 0;
        m_puddleTimer        = 10000;
        m_unstableExperimentTimer = 30000;
        m_unboundPlagueTimer = 10000;
        m_mutatedPlagueTimer = 10000;
        m_chokingGasBombTimer = 35000;
        m_malleableGooTimer  = 25000;
        m_creature->SetSpellList(0);

        // set or remove not selectable flag depending on Festergut and Rotface
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_ROTFACE) != DONE || m_instance->GetData(TYPE_FESTERGUT) != DONE)
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
            else
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
                // Once both observation encounters are complete Putricide is
                // back at his laboratory home position.  Discard any stale
                // balcony point movement so the client does not render him
                // running in place while idle at the table.
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }

        CleanupEncounterAuras();
    }

    void CleanupEncounterAuras()
    {
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            if (Player* player = playerRef.getSource())
            {
                player->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE);
                player->RemoveAurasDueToSpell(SPELL_MUTATED_PLAGUE);
                player->RemoveAurasDueToSpell(SPELL_OOZE_VARIABLE_OOZE);
                player->RemoveAurasDueToSpell(SPELL_GAS_VARIABLE_GAS);
            }
        }

        // Native encounter cleanup removes the player's abomination vehicle
        // and the two laboratory tentacle visuals.  These must run on victory
        // as well as evade or they can keep the room in combat after death.
        m_creature->CastSpell(m_creature, SPELL_CLEANSE_MUTATION, TRIGGERED_OLD_TRIGGERED);
        if (m_instance)
        {
            if (Creature* tentacle = m_instance->GetSingleCreatureFromStorage(NPC_OOZE_TENTACLE_STALKER))
                tentacle->RemoveAllAurasOnEvade();
            if (Creature* tentacle = m_instance->GetSingleCreatureFromStorage(NPC_SLIMY_TENTACLE_STALKER))
                tentacle->RemoveAllAurasOnEvade();
        }
    }

    void CleanupEncounterSummons()
    {
        uint32 const entries[] =
        {
            NPC_GROWING_OOZE_PUDDLE_TRIGGER, NPC_GROWING_OOZE_PUDDLE,
            NPC_GAS_CLOUD, NPC_VOLATILE_OOZE, NPC_CHOKING_GAS_BOMB
        };

        for (uint32 entry : entries)
        {
            CreatureList summons;
            GetCreatureListWithEntryInGrid(summons, m_creature, entry, 150.0f);
            for (Creature* summon : summons)
                if (summon->IsTemporarySummon())
                    summon->ForcedDespawn();
        }
    }

    void ApplyHeroicVariables()
    {
        if (!m_instance || !m_instance->IsHeroicDifficulty() || !m_instance->Is25ManDifficulty())
            return;

        std::vector<Player*> targets;
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                if (player->IsAlive() && !player->IsGameMaster() && m_creature->IsWithinDistInMap(player, 120.0f))
                    targets.push_back(player);

        bool oozeVariable = urand(0, 1) != 0;
        while (!targets.empty())
        {
            size_t index = urand(0, targets.size() - 1);
            Player* player = targets[index];
            player->CastSpell(player, oozeVariable ? SPELL_OOZE_VARIABLE_OOZE : SPELL_GAS_VARIABLE_GAS,
                TRIGGERED_OLD_TRIGGERED);
            oozeVariable = !oozeVariable;
            targets.erase(targets.begin() + index);
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        if (m_instance)
            m_instance->SetData(TYPE_PROFESSOR_PUTRICIDE, IN_PROGRESS);

        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_PROFESSOR_PUTRICIDE, DONE);

        DoBroadcastText(SAY_DEATH, m_creature);
        CleanupEncounterAuras();
        CleanupEncounterSummons();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_PROFESSOR_PUTRICIDE, FAIL);

        CleanupEncounterAuras();
        CleanupEncounterSummons();
    }

    void AttackStart(Unit* who) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        CombatAI::AttackStart(who);
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType != POINT_MOTION_TYPE)
            return;

        // handle phase transition
        if (data == POINT_PUTRICIDE_SPAWN)
        {
            if (!m_instance)
                return;

            DoCastSpellIfCan(m_creature, (m_phase == PHASE_RUNNING_ONE ? SPELL_CREATE_CONCOCTION : SPELL_GUZZLE_POTIONS));

            if (m_instance->IsHeroicDifficulty())
            {
                DoBroadcastText(SAY_PHASE_CHANGE, m_creature);
                ApplyHeroicVariables();
                m_transitionTimer = 30000;
            }
            else
                m_transitionTimer = 15000;

            m_phase = (m_phase == PHASE_RUNNING_ONE ? PHASE_TRANSITION_ONE : PHASE_TRANSITION_TWO);
        }
        else if (data == POINT_FESTERGUT_BALCONY)
            m_creature->SetFacingTo(3.316f);
        else if (data == POINT_ROTFACE_BALCONY)
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
        if (!m_instance)
            return;

        // Unstable experiment
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (Creature* stalker = m_creature->GetMap()->GetCreature(m_instance->GetScientistStalkerGuid(m_isOrange)))
                DoCastSpellIfCan(stalker, m_isOrange ? SPELL_ORANGE_OOZE_SUMMON : SPELL_GREEN_OOZE_SUMMON, CAST_TRIGGERED);

            m_isOrange = !m_isOrange;
        }
        // Volatile Experiment
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (Creature* stalker = m_creature->GetMap()->GetCreature(m_instance->GetScientistStalkerGuid(true)))
                DoCastSpellIfCan(stalker, SPELL_ORANGE_OOZE_SUMMON, CAST_TRIGGERED);
            if (Creature* stalker = m_creature->GetMap()->GetCreature(m_instance->GetScientistStalkerGuid(false)))
                DoCastSpellIfCan(stalker, SPELL_GREEN_OOZE_SUMMON, CAST_TRIGGERED);
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_UNSTABLE_EXPERIMENT)
            DoBroadcastText(EMOTE_EXPERIMENT, m_creature);
    }

    void UpdateAI(const uint32 diff) override
    {
        // no attacking during the Festergut / Rotface encounters
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_instance)
            return;

        // Enrage
        if (m_enrageTimer)
        {
            if (m_enrageTimer <= diff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(SAY_BERSERK, m_creature);
                    m_enrageTimer = 0;
                }
            }
            else
                m_enrageTimer -= diff;
        }

        switch (m_phase)
        {
            case PHASE_TWO:
            case PHASE_THREE:
            {
                // Phase two and three abilities have encounter-specific
                // targeting and summon side effects that cannot be expressed
                // safely through a generic creature spell list.
                if (m_chokingGasBombTimer < diff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CHOKING_GAS_BOMB) == CAST_OK)
                        m_chokingGasBombTimer = 35000;
                }
                else
                    m_chokingGasBombTimer -= diff;

                if (m_malleableGooTimer < diff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MALLEABLE_GOO) == CAST_OK)
                        m_malleableGooTimer = 25000;
                }
                else
                    m_malleableGooTimer -= diff;

                if (m_phase == PHASE_THREE)
                {
                    if (m_mutatedPlagueTimer <= diff)
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MUTATED_PLAGUE) == CAST_OK)
                            m_mutatedPlagueTimer = 10000;
                    }
                    else
                        m_mutatedPlagueTimer -= diff;
                    break;
                }
                // Phase two continues through the shared phase one/two flow.
            }
            // no break
            case PHASE_ONE:
            {
                if (m_transitionTimer)
                {
                    if (m_transitionTimer <= diff)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_PERIODIC, CAST_TRIGGERED);

                        float fX, fY, fZ;
                        SetCombatMovement(false);

                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetRespawnCoord(fX, fY, fZ);
                        m_creature->GetMotionMaster()->MovePoint(POINT_PUTRICIDE_SPAWN, fX, fY, fZ);

                        m_phase = (m_phase == PHASE_ONE ? PHASE_RUNNING_ONE : PHASE_RUNNING_TWO);
                        m_transitionTimer = 0;
                    }
                    else
                        m_transitionTimer -= diff;

                    // Preparing for phase transition
                    return;
                }

                // Unbound Plague
                if (m_instance->IsHeroicDifficulty())
                {
                    if (m_unboundPlagueTimer < diff)
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                                m_unboundPlagueTimer = 70000;
                        }
                    }
                    else
                        m_unboundPlagueTimer -= diff;
                }

                if (m_unstableExperimentTimer < diff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
                        m_unstableExperimentTimer = 37000;
                }
                else
                    m_unstableExperimentTimer -= diff;

                // health check
                if (m_creature->GetHealthPercent() <= (m_phase == PHASE_ONE ? 80.0f : 35.0f))
                {
                    uint32 spellId = (m_instance->IsHeroicDifficulty() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS);

                    if (DoCastSpellIfCan(m_creature, spellId, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        m_transitionTimer = 2500;
                        return;
                    }
                }

                break;
            }
            // Transition phases
            case PHASE_TRANSITION_ONE:
            case PHASE_TRANSITION_TWO:
            {
                if (m_transitionTimer < diff)
                {
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

                    DoBroadcastText((m_phase == PHASE_TRANSITION_ONE ? SAY_TRANSFORM_1 : SAY_TRANSFORM_2), m_creature);

                    // in heroic it changes form at the end of the transition
                    if (!m_instance->IsHeroicDifficulty())
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_CANCEL, CAST_TRIGGERED);
                    else
                    {
                        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
                        {
                            if (Player* player = playerRef.getSource())
                            {
                                player->RemoveAurasDueToSpell(SPELL_OOZE_VARIABLE_OOZE);
                                player->RemoveAurasDueToSpell(SPELL_GAS_VARIABLE_GAS);
                            }
                        }
                    }

                    m_phase = (m_phase == PHASE_TRANSITION_ONE ? PHASE_TWO : PHASE_THREE);
                    if (m_phase == PHASE_THREE)
                    {
                        m_creature->CastSpell(m_creature, SPELL_CLEANSE_MUTATION, TRIGGERED_OLD_TRIGGERED);
                        m_instance->DoToggleGameObjectFlags(GO_DRINK_ME, GO_FLAG_NO_INTERACT, true);
                    }
                    m_transitionTimer = 0;
                }
                else
                    m_transitionTimer -= diff;

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

        // Slime puddles continue throughout active combat phases, but pause
        // while Putricide is moving to or transforming at the laboratory.
        if (m_puddleTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SLIME_PUDDLE_TRIGGER) == CAST_OK)
                m_puddleTimer = 35000;
        }
        else
            m_puddleTimer -= diff;

        CombatAI::UpdateAI(diff);
    }
};

UnitAI* GetAI_boss_professor_putricide(Creature* pCreature)
{
    return new boss_professor_putricideAI(pCreature);
}

// 69795 - Ooze Flood Trigger
struct OozeFloodTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        uint32 spellId = spell->m_spellInfo->Id;
        Unit* target = spell->GetUnitTarget();
        instance_icecrown_citadel* pInstance = dynamic_cast<instance_icecrown_citadel*>(target->GetInstanceData());
        if (!pInstance)
            return;

        // Defind the target as one of the stalkers from the top of the taps
        GuidList lStalkersGuidList;
        pInstance->GetRotfaceStalkersList(lStalkersGuidList);

        std::vector<Creature*> vTapStalkers;
        vTapStalkers.reserve(lStalkersGuidList.size());

        for (GuidList::const_iterator itr = lStalkersGuidList.begin(); itr != lStalkersGuidList.end(); ++itr)
        {
            if (Creature* pStalker = target->GetMap()->GetCreature(*itr))
                vTapStalkers.push_back(pStalker);
        }

        if (vTapStalkers.empty())
        {
            script_error_log("Instance Icecrown Citadel: ERROR Failed to properly find creature %u for Ooze Flood event.", NPC_PUDDLE_STALKER);
            return;
        }

        // pick random target of the tap stalkers
        Creature* pTarget = vTapStalkers[urand(0, vTapStalkers.size() - 1)];
        if (!pTarget)
            return;

        // Get the nearest twin tap stalker.  The old loop simply kept the
        // last stalker returned by the grid visitor, which could pair taps
        // from unrelated flood lanes.
        Creature* pNearTarget = nullptr;
        float fNearDistance = 30.0f;
        CreatureList lTargetsInRange;
        GetCreatureListWithEntryInGrid(lTargetsInRange, pTarget, pTarget->GetEntry(), 30.0f);

        if (lTargetsInRange.empty())
            return;

        // Find only the nearest *other* upper tap trigger.
        for (CreatureList::const_iterator itr = lTargetsInRange.begin(); itr != lTargetsInRange.end(); ++itr)
        {
            float fDistance = pTarget->GetDistance(*itr);
            if (*itr != pTarget && (*itr)->GetPositionZ() > 370.0f && fDistance < fNearDistance)
            {
                pNearTarget = *itr;
                fNearDistance = fDistance;
            }
        }

        if (!pNearTarget)
            return;

        // cast the triggered spell on each target
        target->CastSpell(pTarget, spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(pNearTarget, spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
        DoBroadcastText(urand(0, 1) ? SAY_SLIME_FLOW_1 : SAY_SLIME_FLOW_2, target);
    }
};

// 70351 - Unstable Experiment
struct UnstableExperiment : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), target);
    }
};

// 72840 - Volatile Experiment
struct VolatileExperiment : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, spell->GetCaster(), target);
    }
};

/*######
## npc_volatile_ooze_icc
######*/

struct npc_volatile_ooze_iccAI : public CombatAI
{
    npc_volatile_ooze_iccAI(Creature* creature) : CombatAI(creature, 0) { Reset(); }

    uint32 m_adhesiveTimer;

    ObjectGuid m_targetGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_adhesiveTimer = urand(2000, 4000);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_adhesiveTimer && who->GetTypeId() == TYPEID_PLAYER && who->IsWithinDistInMap(m_creature, ATTACK_DISTANCE) && who->GetObjectGuid() == m_targetGuid)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_OOZE_ERUPTION) == CAST_OK)
            {
                m_adhesiveTimer = urand(3000, 5000);
                m_targetGuid.Clear();
            }
        }

        CombatAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_adhesiveTimer)
        {
            if (m_adhesiveTimer <= diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_OOZE_ADHESIVE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_OOZE_ADHESIVE) == CAST_OK)
                    {
                        m_creature->GetMotionMaster()->MoveFollow(target, ATTACK_DISTANCE * 0.5f, m_creature->GetAngle(target));
                        m_targetGuid = target->GetObjectGuid();
                        m_adhesiveTimer = 0;
                    }
                }
            }
            else
                m_adhesiveTimer -= diff;
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

struct npc_gas_cloud_iccAI : public CombatAI
{
    npc_gas_cloud_iccAI(Creature* creature) : CombatAI(creature, 0) { Reset(); }

    uint32 m_gasBloatTimer;

    ObjectGuid m_targetGuid;

    void Reset() override
    {
        CombatAI::Reset();
        m_gasBloatTimer = urand(2000, 4000);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_gasBloatTimer && who->GetTypeId() == TYPEID_PLAYER && who->IsWithinDistInMap(m_creature, ATTACK_DISTANCE) && who->GetObjectGuid() == m_targetGuid)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EXPUNGED_GAS) == CAST_OK)
            {
                m_gasBloatTimer = urand(3000, 5000);
                m_targetGuid.Clear();
            }
        }

        CombatAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_gasBloatTimer)
        {
            if (m_gasBloatTimer <= diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_OOZE_ADHESIVE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_GASEOUS_BLOAT) == CAST_OK)
                    {
                        m_creature->GetMotionMaster()->MoveFollow(target, ATTACK_DISTANCE * 0.5f, m_creature->GetAngle(target));
                        m_targetGuid = target->GetObjectGuid();
                        m_gasBloatTimer = 0;
                    }
                }
            }
            else
                m_gasBloatTimer -= diff;
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
struct npc_growing_ooze_puddleAI : public CombatAI
{
    npc_growing_ooze_puddleAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
};

UnitAI* GetAI_npc_growing_ooze_puddle(Creature* pCreature)
{
    return new npc_growing_ooze_puddleAI(pCreature);
}

// 70360 - Eat Ooze
struct EatOoze : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        instance_icecrown_citadel* pInstance = dynamic_cast<instance_icecrown_citadel*>(target->GetInstanceData());
        if (!pInstance)
            return;

        uint32 triggerSpellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);
        // ToDo: research if the amount of stacks removal is correct
        target->RemoveAuraHolderFromStack(triggerSpellId, 3);

        // despawn if we don't have the grow aura anymore
        if (!target->HasAura(spell->m_spellInfo->CalculateSimpleValue(effIdx)))
        {
            target->RemoveAllAurasOnEvade();
            if (target->IsCreature())
                static_cast<Creature*>(target)->ForcedDespawn(5000);
        }

        pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NAUSEA, false);
    }
};

/*######
## npc_choking_gas_bomb
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_choking_gas_bombAI : public CombatAI
{
    npc_choking_gas_bombAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
};

UnitAI* GetAI_npc_choking_gas_bomb(Creature* pCreature)
{
    return new npc_choking_gas_bombAI(pCreature);
}

/*######
## npc_puddle_stalker
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_puddle_stalkerAI : public CombatAI
{
    npc_puddle_stalkerAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
};

UnitAI* GetAI_npc_puddle_stalker(Creature* pCreature)
{
    return new npc_puddle_stalkerAI(pCreature);
}

// 69782 - Ooze Flood
struct OozeFlood : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();

        // Set the target manually to the closest stalker on the room floor.
        // A generic closest-entry lookup can select another upper tap and
        // render the ooze flood suspended in mid-air.
        Creature* pFloorStalker = nullptr;
        float fNearDistance = 20.0f;
        CreatureList lTargetsInRange;
        GetCreatureListWithEntryInGrid(lTargetsInRange, target, target->GetEntry(), 20.0f);

        for (CreatureList::const_iterator itr = lTargetsInRange.begin(); itr != lTargetsInRange.end(); ++itr)
        {
            float fDistance = target->GetDistance(*itr);
            if ((*itr)->GetPositionZ() < 365.0f && fDistance < fNearDistance)
            {
                pFloorStalker = *itr;
                fNearDistance = fDistance;
            }
        }

        if (pFloorStalker)
            target->CastSpell(pFloorStalker, spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_professor_putricide()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_professor_putricide";
    pNewScript->GetAI = &GetAI_boss_professor_putricide;
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
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_choking_gas_bomb";
    pNewScript->GetAI = GetAI_npc_choking_gas_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_puddle_stalker";
    pNewScript->GetAI = GetAI_npc_puddle_stalker;
    pNewScript->RegisterSelf();

    RegisterSpellScript<OozeFloodTrigger>("spell_ooze_flood_trigger");
    RegisterSpellScript<UnstableExperiment>("spell_unstable_experiment");
    RegisterSpellScript<VolatileExperiment>("spell_volatile_experiment");
    RegisterSpellScript<EatOoze>("spell_eat_ooze");
    RegisterSpellScript<OozeFlood>("spell_ooze_flood");
}
