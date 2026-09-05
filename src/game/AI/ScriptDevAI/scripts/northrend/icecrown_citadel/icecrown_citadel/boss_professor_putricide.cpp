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
#include <cmath>

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

    EMOTE_CHOKING_GAS_BOMB      = 38498,
    EMOTE_MALLEABLE_GOO         = 38499,
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
    SPELL_SLIME_PUDDLE_TRIGGER      = 70341,                // triggers 70342 to summon 37690

    SPELL_UNSTABLE_EXPERIMENT       = 70351,                // triggers 71412 or 71415 alternatively

    // Phase 2 spells
    SPELL_CREATE_CONCOCTION         = 71621,                // transform spell
    SPELL_MALLEABLE_GOO             = 70852,
    SPELL_CHOKING_GAS_BOMB          = 71255,                // triggers 71273

    // Tear Gas - phase transitions
    SPELL_TEAR_GAS                  = 71617,                // initial tear gas spell on transition start
    SPELL_TEAR_GAS_CREATURE         = 71618,
    SPELL_TEAR_GAS_CANCEL           = 71620,
    SPELL_TEAR_GAS_PERIODIC         = 73170,                // putricide continues to have the periodic aura during transition
    SPELL_VOLATILE_EXPERIMENT       = 72840,                // heroic transition spell; triggers 71412 and 71415 at once

    // Phase 3 transition and spell
    SPELL_CLEANSE_MUTATION          = 71693,                // cancel abomination
    SPELL_GUZZLE_POTIONS            = 71893,                // triggers 71704 and 71621
    SPELL_MUTATED_PLAGUE            = 72451,
    SPELL_MUTATED_PLAGUE_CLEAR      = 72618,

    // heroic spells
    SPELL_UNBOUND_PLAGUE            = 70911,
    SPELL_UNBOUND_PLAGUE_SEARCHER   = 70917,
    SPELL_PLAGUE_SICKNESS           = 70953,
    SPELL_UNBOUND_PLAGUE_PROTECTION = 70955,
    SPELL_OOZE_VARIABLE             = 70352,                // ToDo: research how this should work
    SPELL_OOZE_VARIABLE_OOZE        = 74118,
    SPELL_GAS_VARIABLE              = 70353,                // ToDo: research how this should work
    SPELL_GAS_VARIABLE_GAS          = 74119,

    SPELL_OOZE_TANK_PROTECTION      = 71770,

    // spells used for other encounters
    SPELL_OOZE_FLOOD_TRIGGER        = 69795,                // triggers 69782 on top pipes targets - 37013
    SPELL_OOZE_FLOOD                = 69782,                // triggers 69783 on the closest lower pipe target - 37013

    // Summoned spells
    // growing ooze puddle
    SPELL_SLIME_PUDDLE_DAMAGE       = 70343,
    SPELL_GROW_STACKER              = 70345,                // triggers 70347
    SPELL_GROW                      = 70347,
    SPELL_EAT_OOZE                  = 70360,
    SPELL_REGURGITATED_OOZE          = 70539,

    // choking gas bomb
    SPELL_CHOKING_GAS_PERIODIC      = 71259,
    SPELL_CHOKING_GAS_EXPLOSION_PER = 71280,

    // Volatile Ooze
    SPELL_OOZE_ADHESIVE             = 70447,                // Cast on player and follow the player to errupt
    SPELL_OOZE_ERUPTION             = 70492,
    SPELL_VOLATILE_OOZE_PROTECTION  = 70530,

    // Gas cloud
    SPELL_GASEOUS_BLOAT             = 70672,                // Chase players and expunge gas
    SPELL_EXPUNGED_GAS              = 70701,
    SPELL_GASEOUS_BLOAT_PROTECTION  = 70812,
    SPELL_GASEOUS_BLOAT_VISUAL      = 70215,

    // Mutated Abomination
    SPELL_TRANSFORMATION_NAME         = 72401,
    SPELL_TRANSFORMATION_DAMAGE       = 70405,
    SPELL_POWER_DRAIN                 = 70385,

    // Unholy Infusion
    SPELL_SHADOWS_FATE                = 71169,
    SPELL_SHADOW_INFUSION             = 71516,
    SPELL_UNHOLY_INFUSION_CREDIT      = 71518,
    QUEST_UNHOLY_INFUSION             = 24749,
    ITEM_SHADOWS_EDGE                 = 49888,

    SPELL_LIST_ABOMINATION_STANDARD   = 3767201,
    SPELL_LIST_ABOMINATION_INFUSION   = 3767202,

    // npcs
    NPC_GROWING_OOZE_PUDDLE         = 37690,
    NPC_GAS_CLOUD                   = 37562,
    NPC_VOLATILE_OOZE               = 37697,
    NPC_CHOKING_GAS_BOMB            = 38159,
    NPC_MUTATED_ABOMINATION_10      = 37672,
    NPC_MUTATED_ABOMINATION_25      = 38285,
};

bool IsMutatedAbomination(uint32 entry)
{
    return entry == NPC_MUTATED_ABOMINATION_10 || entry == NPC_MUTATED_ABOMINATION_25;
}

bool IsPutricide25Man(Unit const* unit)
{
    if (!unit || !unit->GetMap()->IsDungeon())
        return false;

    Difficulty const difficulty = unit->GetMap()->GetDifficulty();
    return difficulty == RAID_DIFFICULTY_25MAN_NORMAL || difficulty == RAID_DIFFICULTY_25MAN_HEROIC;
}

uint32 GetPutricideDifficultySpellId(Unit const* unit, uint32 spellId)
{
    if (!unit)
        return spellId;

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (spellInfo && spellInfo->SpellDifficultyId && unit->GetMap()->IsDungeon())
        if (SpellEntry const* difficultySpell = GetSpellEntryByDifficulty(spellInfo->SpellDifficultyId,
                unit->GetMap()->GetDifficulty(), unit->GetMap()->IsRaid()))
            return difficultySpell->Id;

    return spellId;
}

void RemovePutricideDifficultyAura(Unit* target, uint32 spellId)
{
    if (target)
        target->RemoveAurasDueToSpell(GetPutricideDifficultySpellId(target, spellId));
}

bool IsGaseousBloatSpell(uint32 spellId)
{
    return spellId == 70672 || spellId == 72455 || spellId == 72832 || spellId == 72833;
}

/*######
## npc_mutated_abomination
######*/

// The abomination is a player-controlled encounter piece.  As with the
// Karazhan chess pieces, normal creature combat and evade handling must not
// run: its periodic raid-damage pulse can otherwise put it in combat without
// a victim, immediately enter evade, and remove the transformation aura.
struct npc_mutated_abominationAI : public CombatAI
{
    npc_mutated_abominationAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(true);
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }
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

enum PutricideActions
{
    PUTRICIDE_ACTION_PHASE_CHANGE,
    PUTRICIDE_ACTION_BERSERK,
    PUTRICIDE_ACTION_SLIME_PUDDLE,
    PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT,
    PUTRICIDE_ACTION_UNBOUND_PLAGUE,
    PUTRICIDE_ACTION_CHOKING_GAS_BOMB,
    PUTRICIDE_ACTION_MALLEABLE_GOO,
    PUTRICIDE_ACTION_MUTATED_PLAGUE,
    PUTRICIDE_ACTIONS_MAX,
    PUTRICIDE_ACTION_PHASE_TRANSITION = PUTRICIDE_ACTIONS_MAX,
};

enum PutricideAddActions
{
    PUTRICIDE_ADD_ACTION_TARGET,
};

/*######
## boss_professor_putricide
######*/

struct boss_professor_putricideAI : public CombatAI
{
    boss_professor_putricideAI(Creature* creature) : CombatAI(creature, PUTRICIDE_ACTIONS_MAX),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(PUTRICIDE_ACTION_PHASE_CHANGE, true);
        AddCombatAction(PUTRICIDE_ACTION_BERSERK, true);
        AddCombatAction(PUTRICIDE_ACTION_SLIME_PUDDLE, true);
        AddCombatAction(PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT, true);
        AddCombatAction(PUTRICIDE_ACTION_UNBOUND_PLAGUE, true);
        AddCombatAction(PUTRICIDE_ACTION_CHOKING_GAS_BOMB, true);
        AddCombatAction(PUTRICIDE_ACTION_MALLEABLE_GOO, true);
        AddCombatAction(PUTRICIDE_ACTION_MUTATED_PLAGUE, true);
        AddCustomAction(PUTRICIDE_ACTION_PHASE_TRANSITION, true, [&]() { HandlePhaseTransition(); }, TIMER_COMBAT_COMBAT);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    bool m_isOrange;
    Phase m_phase;

    void Reset() override
    {
        CombatAI::Reset();
        m_isOrange = urand(0, 1) != 0;
        m_phase = PHASE_ONE;
        m_creature->UpdateSpeed(MOVE_RUN, false);
        m_creature->SetSpellList(0);

        SetEncounterAvailability();
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();

        // Putricide observes Festergut and Rotface while passive.  The generic
        // CMaNGOS evade path deliberately does not move passive creatures
        // home, so complete this observer-specific return after Reset() has
        // restored the appropriate encounter reaction state.
        if (m_creature->IsAlive() && !m_creature->IsBoarded())
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    void SetEncounterAvailability()
    {
        bool const available = m_instance && m_instance->GetData(TYPE_ROTFACE) == DONE &&
            m_instance->GetData(TYPE_FESTERGUT) == DONE;

        SetCombatMovement(available);
        SetMeleeEnabled(available);
        SetCombatScriptStatus(!available);
        SetReactState(available ? REACT_AGGRESSIVE : REACT_PASSIVE);
        m_creature->SetCanEnterCombat(available);

        if (!available)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
            return;
        }

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void CleanupEncounterAuras()
    {
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            if (Player* player = playerRef.getSource())
            {
                RemovePutricideDifficultyAura(player, SPELL_UNBOUND_PLAGUE);
                RemovePutricideDifficultyAura(player, SPELL_MUTATED_PLAGUE);
                player->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE_SEARCHER);
                player->RemoveAurasDueToSpell(SPELL_PLAGUE_SICKNESS);
                player->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE_PROTECTION);
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
            NPC_GROWING_OOZE_PUDDLE, NPC_GAS_CLOUD,
            NPC_VOLATILE_OOZE, NPC_CHOKING_GAS_BOMB,
            NPC_MUTATED_ABOMINATION_10, NPC_MUTATED_ABOMINATION_25
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

    void RemoveMutatedAbominations()
    {
        uint32 const entries[] = { NPC_MUTATED_ABOMINATION_10, NPC_MUTATED_ABOMINATION_25 };
        for (uint32 entry : entries)
        {
            CreatureList abominations;
            GetCreatureListWithEntryInGrid(abominations, m_creature, entry, 150.0f);
            for (Creature* abomination : abominations)
                RemovePutricideDifficultyAura(abomination, SPELL_TRANSFORMATION_DAMAGE);
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
        if (!m_instance)
            return;

        m_instance->SetData(TYPE_PROFESSOR_PUTRICIDE, IN_PROGRESS);
        DoBroadcastText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_OOZE_TANK_PROTECTION, CAST_TRIGGERED);
        m_creature->SetInCombatWithZone();
        ResetCombatAction(PUTRICIDE_ACTION_BERSERK, 10min);
        PreparePhaseActions();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_PROFESSOR_PUTRICIDE, DONE);

        DoBroadcastText(SAY_DEATH, m_creature);
        if (m_instance && m_instance->Is25ManDifficulty() && m_creature->HasAura(SPELL_SHADOWS_FATE))
            m_creature->CastSpell(m_creature, SPELL_UNHOLY_INFUSION_CREDIT, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(m_creature, SPELL_MUTATED_PLAGUE_CLEAR, TRIGGERED_OLD_TRIGGERED);
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

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType != POINT_MOTION_TYPE)
            return;

        // handle phase transition
        if (data == POINT_PUTRICIDE_SPAWN)
        {
            m_creature->UpdateSpeed(MOVE_RUN, false);

            if (!m_instance)
                return;

            DoCastSpellIfCan(m_creature, (m_phase == PHASE_RUNNING_ONE ? SPELL_CREATE_CONCOCTION : SPELL_GUZZLE_POTIONS));

            if (m_instance->IsHeroicDifficulty())
            {
                DoBroadcastText(SAY_PHASE_CHANGE, m_creature);
                ApplyHeroicVariables();
                ResetTimer(PUTRICIDE_ACTION_PHASE_TRANSITION, 30s);
            }
            else
                ResetTimer(PUTRICIDE_ACTION_PHASE_TRANSITION, 15s);

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
            case NPC_GROWING_OOZE_PUDDLE:
                pSummoned->CastSpell(pSummoned, SPELL_GROW_STACKER, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_SLIME_PUDDLE_DAMAGE, TRIGGERED_OLD_TRIGGERED);
                for (uint8 i = 0; i < 7; ++i)
                    pSummoned->CastSpell(pSummoned, SPELL_GROW, TRIGGERED_OLD_TRIGGERED);
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
        else if (spellInfo->Id == SPELL_CHOKING_GAS_BOMB)
            DoBroadcastText(EMOTE_CHOKING_GAS_BOMB, m_creature);
    }

    void DisablePhaseActions()
    {
        SetActionReadyStatus(PUTRICIDE_ACTION_PHASE_CHANGE, false);
        DisableCombatAction(PUTRICIDE_ACTION_SLIME_PUDDLE);
        DisableCombatAction(PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT);
        DisableCombatAction(PUTRICIDE_ACTION_UNBOUND_PLAGUE);
        DisableCombatAction(PUTRICIDE_ACTION_CHOKING_GAS_BOMB);
        DisableCombatAction(PUTRICIDE_ACTION_MALLEABLE_GOO);
        DisableCombatAction(PUTRICIDE_ACTION_MUTATED_PLAGUE);
    }

    void PreparePhaseActions()
    {
        DisablePhaseActions();
        SetActionReadyStatus(PUTRICIDE_ACTION_PHASE_CHANGE, true);

        ResetCombatAction(PUTRICIDE_ACTION_SLIME_PUDDLE, m_phase == PHASE_ONE ? 10s : 35s);
        switch (m_phase)
        {
            case PHASE_ONE:
                ResetCombatAction(PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT, 30s);
                if (m_instance->IsHeroicDifficulty())
                    ResetCombatAction(PUTRICIDE_ACTION_UNBOUND_PLAGUE, 20s);
                break;
            case PHASE_TWO:
                ResetCombatAction(PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT, urand(35000, 40000));
                if (m_instance->IsHeroicDifficulty())
                    ResetCombatAction(PUTRICIDE_ACTION_UNBOUND_PLAGUE, 90s);
                ResetCombatAction(PUTRICIDE_ACTION_CHOKING_GAS_BOMB, urand(35000, 40000));
                ResetCombatAction(PUTRICIDE_ACTION_MALLEABLE_GOO, urand(21000, 26000));
                break;
            case PHASE_THREE:
                ResetCombatAction(PUTRICIDE_ACTION_CHOKING_GAS_BOMB, urand(35000, 40000));
                ResetCombatAction(PUTRICIDE_ACTION_MALLEABLE_GOO, urand(25000, 30000));
                ResetCombatAction(PUTRICIDE_ACTION_MUTATED_PLAGUE, 10s);
                break;
            default:
                break;
        }
    }

    void StartPhaseTransition()
    {
        uint32 const spellId = m_instance->IsHeroicDifficulty() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS;
        if (DoCastSpellIfCan(m_creature, spellId, CAST_INTERRUPT_PREVIOUS) != CAST_OK)
            return;

        // Retail transition movement is twice Putricide's normal run speed.
        // Regurgitated Ooze can still slow him normally; its 50% snare then
        // reduces this transition run to the regular movement rate.
        m_creature->UpdateSpeed(MOVE_RUN, false, 2.0f);
        DisablePhaseActions();
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        ResetTimer(PUTRICIDE_ACTION_PHASE_TRANSITION, 2500ms);
    }

    void HandlePhaseTransition()
    {
        if (m_phase == PHASE_ONE || m_phase == PHASE_TWO)
        {
            DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_PERIODIC, CAST_TRIGGERED);

            float x, y, z;
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetRespawnCoord(x, y, z);
            m_creature->GetMotionMaster()->MovePoint(POINT_PUTRICIDE_SPAWN, x, y, z, FORCED_MOVEMENT_RUN);
            m_phase = m_phase == PHASE_ONE ? PHASE_RUNNING_ONE : PHASE_RUNNING_TWO;
            return;
        }

        if (m_phase != PHASE_TRANSITION_ONE && m_phase != PHASE_TRANSITION_TWO)
            return;

        DoBroadcastText(m_phase == PHASE_TRANSITION_ONE ? SAY_TRANSFORM_1 : SAY_TRANSFORM_2, m_creature);

        m_creature->RemoveAurasDueToSpell(SPELL_TEAR_GAS_PERIODIC);

        if (!m_instance->IsHeroicDifficulty())
            DoCastSpellIfCan(nullptr, SPELL_TEAR_GAS_CANCEL, CAST_TRIGGERED);
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

        m_phase = m_phase == PHASE_TRANSITION_ONE ? PHASE_TWO : PHASE_THREE;
        if (m_phase == PHASE_THREE)
        {
            RemoveMutatedAbominations();
            m_instance->DoToggleGameObjectFlags(GO_DRINK_ME, GO_FLAG_NO_INTERACT, true);
        }

        SetCombatMovement(true);
        SetMeleeEnabled(true);
        m_creature->GetMotionMaster()->Clear();
        if (Unit* victim = m_creature->GetVictim())
            m_creature->GetMotionMaster()->MoveChase(victim);
        PreparePhaseActions();
        SetCombatScriptStatus(false);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case PUTRICIDE_ACTION_PHASE_CHANGE:
                if ((m_phase == PHASE_ONE && m_creature->GetHealthPercent() <= 80.0f) ||
                    (m_phase == PHASE_TWO && m_creature->GetHealthPercent() <= 35.0f))
                    StartPhaseTransition();
                break;
            case PUTRICIDE_ACTION_BERSERK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case PUTRICIDE_ACTION_SLIME_PUDDLE:
            {
                std::vector<Unit*> targets;
                m_creature->SelectAttackingTargets(targets, ATTACKING_TARGET_ALL_SUITABLE, 0, nullptr, SELECT_FLAG_PLAYER);

                uint32 puddles = 0;
                while (!targets.empty() && puddles < 2)
                {
                    uint32 index = urand(0, targets.size() - 1);
                    DoCastSpellIfCan(targets[index], SPELL_SLIME_PUDDLE_TRIGGER, CAST_TRIGGERED);
                    targets.erase(targets.begin() + index);
                    ++puddles;
                }

                if (puddles)
                    ResetCombatAction(action, 35s);
                break;
            }
            case PUTRICIDE_ACTION_UNSTABLE_EXPERIMENT:
                if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
                    ResetCombatAction(action, urand(35000, 40000));
                break;
            case PUTRICIDE_ACTION_UNBOUND_PLAGUE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                    {
                        m_creature->CastSpell(target, SPELL_UNBOUND_PLAGUE_SEARCHER, TRIGGERED_OLD_TRIGGERED);
                        ResetCombatAction(action, 90s);
                    }
                break;
            case PUTRICIDE_ACTION_CHOKING_GAS_BOMB:
                if (DoCastSpellIfCan(m_creature, SPELL_CHOKING_GAS_BOMB) == CAST_OK)
                    ResetCombatAction(action, urand(35000, 40000));
                break;
            case PUTRICIDE_ACTION_MALLEABLE_GOO:
            {
                std::vector<Unit*> targets;
                m_creature->SelectAttackingTargets(targets, ATTACKING_TARGET_ALL_SUITABLE, 0, nullptr,
                    SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK);

                for (auto itr = targets.begin(); itr != targets.end();)
                {
                    Unit* target = *itr;
                    if (!target->IsAlive() || static_cast<Player*>(target)->IsGameMaster() ||
                        m_creature->IsWithinDistInMap(target, 7.0f))
                        itr = targets.erase(itr);
                    else
                        ++itr;
                }

                uint32 maxTargets = 1;
                if (IsPutricide25Man(m_creature))
                    maxTargets = m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC ? 3 : 2;

                uint32 casts = 0;
                while (!targets.empty() && casts < maxTargets)
                {
                    size_t const index = urand(0, targets.size() - 1);
                    m_creature->CastSpell(targets[index], SPELL_MALLEABLE_GOO, TRIGGERED_OLD_TRIGGERED);
                    targets.erase(targets.begin() + index);
                    ++casts;
                }

                if (casts)
                    DoBroadcastText(EMOTE_MALLEABLE_GOO, m_creature);

                ResetCombatAction(action, urand(25000, 30000));
                break;
            }
            case PUTRICIDE_ACTION_MUTATED_PLAGUE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MUTATED_PLAGUE) == CAST_OK)
                    ResetCombatAction(action, 10s);
                break;
        }
    }
};

// 69795 - Ooze Flood Trigger
struct OozeFloodTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
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

// 70346, 72456, 72868, 72869 - Slime Puddle
struct PutricideSlimePuddle : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return false;

        float radius = 2.5f * caster->GetObjectScale();
        return caster->GetDistance2d(target->GetPositionX(), target->GetPositionY(), DIST_CALC_NONE) <= radius * radius;
    }
};

Unit* SelectPutricideOozeTarget(Creature* creature, uint32 channelSpellId)
{
    uint32 const protectionSpellId = channelSpellId == SPELL_OOZE_ADHESIVE ?
        SPELL_VOLATILE_OOZE_PROTECTION : SPELL_GASEOUS_BLOAT_PROTECTION;

    std::vector<Unit*> targets;
    for (auto& playerRef : creature->GetMap()->GetPlayers())
    {
        Player* player = playerRef.getSource();
        if (!player || !player->IsAlive() || player->IsGameMaster() || !creature->CanAttack(player) ||
            !creature->IsWithinDistInMap(player, 120.0f) ||
            player->HasAuraOfDifficulty(protectionSpellId))
            continue;

        targets.push_back(player);
    }

    return targets.empty() ? nullptr : targets[urand(0, targets.size() - 1)];
}

/*######
## npc_volatile_ooze_icc
######*/

struct npc_putricide_oozeAI : public CombatAI
{
    npc_putricide_oozeAI(Creature* creature, uint32 channelSpellId, uint32 explosionSpellId) :
        CombatAI(creature, 0), m_channelSpellId(channelSpellId), m_explosionSpellId(explosionSpellId)
    {
        AddCustomAction(PUTRICIDE_ADD_ACTION_TARGET, 2s, 4s, [&]() { SelectTarget(); }, TIMER_ALWAYS);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        Reset();
    }

    ObjectGuid m_targetGuid;
    uint32 m_channelSpellId;
    uint32 m_explosionSpellId;

    bool IsFixationTarget(Unit const* target) const
    {
        return target && target->GetObjectGuid() == m_targetGuid;
    }

    void StartFixation(Unit* target)
    {
        if (!IsFixationTarget(target))
            return;

        m_creature->DeleteThreatList();
        SetReactState(REACT_AGGRESSIVE);
        SetCombatMovement(true);
        AttackStart(target);
        m_creature->AddThreat(target, 500000000.0f);
    }

    void ResetFixation(uint32 delay = 1000)
    {
        if (Spell* channel = m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (channel->m_spellInfo->Id == m_channelSpellId ||
                channel->m_spellInfo->Id == GetPutricideDifficultySpellId(m_creature, m_channelSpellId))
                m_creature->FinishSpell(CURRENT_CHANNELED_SPELL);

        SetReactState(REACT_PASSIVE);
        m_creature->AttackStop();
        m_creature->DeleteThreatList();
        SetCombatMovement(false, true);

        // Discard the completed fixation's chase generator. Merely placing
        // idle movement on top leaves the old chase in the movement stack and
        // lets it resume while the next target spell is being prepared.
        m_creature->StopMoving();
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_targetGuid.Clear();

        if (delay)
            ResetTimer(PUTRICIDE_ADD_ACTION_TARGET, delay);
        else
            DisableTimer(PUTRICIDE_ADD_ACTION_TARGET);
    }

    void RestartTargeting()
    {
        // Finish removing the expired Bloat aura before beginning its new
        // cast. Starting it from inside the final aura tick lets the outer
        // channel cleanup interrupt the freshly-created spell in CMaNGOS.
        ResetFixation(1);
    }

    void Reset() override
    {
        CombatAI::Reset();
        m_targetGuid.Clear();
        SetCombatScriptStatus(true);
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            StartFixation(invoker);
    }

    void OnSpellInterrupt(SpellEntry const* spellInfo) override
    {
        CombatAI::OnSpellInterrupt(spellInfo);
        if (spellInfo && m_targetGuid &&
            (spellInfo->Id == m_channelSpellId ||
             spellInfo->Id == GetPutricideDifficultySpellId(m_creature, m_channelSpellId)))
            ResetFixation();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (m_targetGuid && who->GetTypeId() == TYPEID_PLAYER &&
            who->IsWithinDistInMap(m_creature, ATTACK_DISTANCE) && who->GetObjectGuid() == m_targetGuid)
        {
            bool exploded = false;
            if (m_explosionSpellId == SPELL_EXPUNGED_GAS)
            {
                uint32 const bloatId = GetPutricideDifficultySpellId(who, SPELL_GASEOUS_BLOAT);
                if (SpellAuraHolder* holder = who->GetSpellAuraHolder(bloatId, m_creature->GetObjectGuid()))
                {
                    uint32 const stacks = holder->GetStackAmount();
                    int32 const factor = IsPutricide25Man(m_creature) ? 1500 : 1250;
                    int32 const damage = factor * int32(stacks * (stacks + 1) / 2);
                    m_creature->CastCustomSpell(m_creature, SPELL_EXPUNGED_GAS, &damage, nullptr, nullptr,
                        TRIGGERED_OLD_TRIGGERED);
                    exploded = true;
                }
            }
            else
            {
                uint32 const adhesiveId = GetPutricideDifficultySpellId(who, SPELL_OOZE_ADHESIVE);
                if (who->GetSpellAuraHolder(adhesiveId, m_creature->GetObjectGuid()) &&
                    DoCastSpellIfCan(m_creature, m_explosionSpellId) == CAST_OK)
                    exploded = true;
            }

            if (exploded)
            {
                who->RemoveAurasByCasterSpell(GetPutricideDifficultySpellId(who, m_channelSpellId),
                    m_creature->GetObjectGuid());
                ResetFixation();
            }
        }

        CombatAI::MoveInLineOfSight(who);
    }

    void SelectTarget()
    {
        if (m_creature->HasAura(SPELL_TEAR_GAS_CREATURE))
        {
            ResetTimer(PUTRICIDE_ADD_ACTION_TARGET, 1s);
            return;
        }

        Unit* target = SelectPutricideOozeTarget(m_creature, m_channelSpellId);
        if (!target)
        {
            ResetTimer(PUTRICIDE_ADD_ACTION_TARGET, 1s);
            return;
        }

        SpellCastArgs args;
        args.SetTarget(m_creature).SetScriptValue(target->GetObjectGuid().GetRawValue());
        m_targetGuid = target->GetObjectGuid();
        if (m_creature->CastSpell(args, m_channelSpellId, TRIGGERED_NONE) == SPELL_CAST_OK)
            return;

        m_targetGuid.Clear();
        ResetTimer(PUTRICIDE_ADD_ACTION_TARGET, 1s);
    }
};

struct npc_volatile_ooze_iccAI : public npc_putricide_oozeAI
{
    npc_volatile_ooze_iccAI(Creature* creature) :
        npc_putricide_oozeAI(creature, SPELL_OOZE_ADHESIVE, SPELL_OOZE_ERUPTION) { }
};

/*######
## npc_gas_cloud_icc
######*/

struct npc_gas_cloud_iccAI : public npc_putricide_oozeAI
{
    npc_gas_cloud_iccAI(Creature* creature) :
        npc_putricide_oozeAI(creature, SPELL_GASEOUS_BLOAT, SPELL_EXPUNGED_GAS) { }
};

// 70447, 72836, 72837, 72838 - Volatile Ooze Adhesive
// 70672, 72455, 72832, 72833 - Gaseous Bloat
struct PutricideOozeChannel : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effIdx*/) const override
    {
        return target && target->IsPlayer() &&
            target->GetObjectGuid() == ObjectGuid(spell->GetScriptValue());
    }

    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (missInfo != SPELL_MISS_NONE || !caster || !target || !caster->AI())
            return;

        caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, caster);
    }

    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const override
    {
        if (IsGaseousBloatSpell(holder->GetId()))
            holder->PresetAuraStacks(10);
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_0 || !IsGaseousBloatSpell(aura->GetId()))
            return;

        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        uint32 const spellId = aura->GetId();
        ObjectGuid const casterGuid = aura->GetCasterGuid();
        target->RemoveAuraHolderFromStack(spellId, 1, casterGuid);

        if (!target->HasAura(spellId) && caster && caster->IsCreature() && caster->AI())
            if (npc_putricide_oozeAI* oozeAI = dynamic_cast<npc_putricide_oozeAI*>(caster->AI()))
                oozeAI->RestartTargeting();
    }
};

// 70308 - Mutated Transformation
struct PutricideMutationInit : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster || !caster->IsPlayer())
            return SPELL_FAILED_BAD_TARGETS;

        instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(caster->GetInstanceData());
        if (!instance || instance->GetData(TYPE_PROFESSOR_PUTRICIDE) != IN_PROGRESS)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        uint32 const entries[] = { NPC_MUTATED_ABOMINATION_10, NPC_MUTATED_ABOMINATION_25 };
        for (uint32 entry : entries)
        {
            CreatureList abominations;
            GetCreatureListWithEntryInGrid(abominations, caster, entry, 150.0f);
            for (Creature* abomination : abominations)
                if (abomination->IsAlive())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        }

        return SPELL_CAST_OK;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        Unit* target = aura->GetTarget();
        if (!target || !target->GetMap()->IsDungeon())
            return;

        uint32 const spellId = IsPutricide25Man(target) ? 71503 : 70311;
        target->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),
            spellId, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
    }
};

// 70311, 71503 - Mutated Transformation
struct PutricideMutatedTransformation : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster || !caster->IsPlayer() || !summon || !IsMutatedAbomination(summon->GetEntry()) ||
            !summon->GetVehicleInfo() || !summon->IsTemporarySummon())
            return;

        Player* player = static_cast<Player*>(caster);
        bool infusionEligible = false;
        if (IsPutricide25Man(player) && player->GetQuestStatus(QUEST_UNHOLY_INFUSION) == QUEST_STATUS_INCOMPLETE)
            if (Item* weapon = player->GetWeaponForAttack(BASE_ATTACK))
                infusionEligible = weapon->GetEntry() == ITEM_SHADOWS_EDGE;

        summon->SetSpellList(infusionEligible ? SPELL_LIST_ABOMINATION_INFUSION : SPELL_LIST_ABOMINATION_STANDARD);

        summon->CastSpell(summon, SPELL_POWER_DRAIN, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(summon, SPELL_TRANSFORMATION_DAMAGE, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(summon, SPELL_TRANSFORMATION_NAME, TRIGGERED_OLD_TRIGGERED);

        summon->SetPowerType(POWER_ENERGY);
        summon->SetMaxPower(POWER_ENERGY, 100);
        summon->SetPower(POWER_ENERGY, 0);
        summon->SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, 0.0f);
        summon->SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, 0.0f);
    }
};

// 70405, 72508, 72509, 72510 - Mutated Transformation dismiss
struct PutricideMutatedTransformationDismiss : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
            return;

        Unit* target = aura->GetTarget();
        if (target && target->IsVehicle() && target->IsCreature())
        {
            target->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
            static_cast<Creature*>(target)->ForcedDespawn();
        }
    }
};

// 70402, 72511, 72512, 72513 - Mutated Transformation damage
struct PutricideMutatedTransformationDamage : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        return caster && target && target->GetObjectGuid() != caster->GetOwnerGuid();
    }
};

// 71693 - Cleanse Mutation
struct PutricideCleanseMutation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
            RemovePutricideDifficultyAura(spell->GetUnitTarget(), SPELL_TRANSFORMATION_DAMAGE);
    }
};

// 71620 - Tear Gas Cancel; 72618 - Mutated Plague Clear
struct PutricideClearAuras : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || effIdx != EFFECT_INDEX_0)
            return;

        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            uint32 const auraId = spell->m_spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
            if (auraId)
                RemovePutricideDifficultyAura(target, auraId);
        }
    }
};

// 71255 - Choking Gas Bomb
struct PutricideChokingGasBomb : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster || effIdx != EFFECT_INDEX_0)
            return;

        uint32 const skip = urand(0, MAX_EFFECT_INDEX - 1);
        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (i == skip)
                continue;

            uint32 const spellId = spell->m_spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
            if (spellId)
                caster->CastSpell(caster, spellId, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr,
                    caster->GetObjectGuid());
        }
    }
};

// 72295 - Malleable Goo selector
struct PutricideMalleableGoo : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        uint32 targets = 1;
        if (IsPutricide25Man(caster))
            targets = caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC ? 3 : 2;
        spell->SetMaxAffectedTargets(targets);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* caster = spell->GetCaster();
        return caster && target && target->IsPlayer() && target->IsAlive() &&
            !static_cast<Player*>(target)->IsGameMaster() && target != caster->GetVictim() &&
            !caster->IsWithinDistInMap(target, 7.0f) && caster->CanAttack(target);
    }
};

// 70920 - Unbound Plague search and transfer
struct PutricideUnboundPlague : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* caster = spell->GetCaster();
        if (!caster || !target || !target->IsPlayer() || target == caster || !target->IsAlive())
            return false;

        if (Aura* searcher = caster->GetAura(SPELL_UNBOUND_PLAGUE_SEARCHER, EFFECT_INDEX_0))
            if (searcher->GetAuraTicks() < 2)
                return false;

        return !target->HasAuraOfDifficulty(SPELL_UNBOUND_PLAGUE) &&
            !target->HasAura(SPELL_UNBOUND_PLAGUE_PROTECTION);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* oldCarrier = spell->GetCaster();
        Unit* newCarrier = spell->GetUnitTarget();
        if (!oldCarrier || !newCarrier || effIdx != EFFECT_INDEX_0)
            return;

        instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(oldCarrier->GetInstanceData());
        Creature* professor = instance ? instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE) : nullptr;
        if (!professor)
            return;

        uint32 const plagueId = GetPutricideDifficultySpellId(oldCarrier, SPELL_UNBOUND_PLAGUE);
        SpellAuraHolder* oldPlague = oldCarrier->GetSpellAuraHolder(plagueId, professor->GetObjectGuid());
        if (!oldPlague || newCarrier->HasAura(plagueId))
            return;

        int32 const maxDuration = oldPlague->GetAuraMaxDuration();
        int32 const duration = oldPlague->GetAuraDuration();
        professor->CastSpell(newCarrier, SPELL_UNBOUND_PLAGUE, TRIGGERED_OLD_TRIGGERED);

        SpellAuraHolder* newPlague = newCarrier->GetSpellAuraHolder(plagueId, professor->GetObjectGuid());
        if (!newPlague)
            return;

        newPlague->SetAuraMaxDuration(maxDuration);
        newPlague->SetAuraDuration(duration);
        newPlague->SendAuraUpdate(false);
        oldCarrier->RemoveAurasDueToSpell(plagueId);
        oldCarrier->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE_SEARCHER);
        oldCarrier->CastSpell(oldCarrier, SPELL_PLAGUE_SICKNESS, TRIGGERED_OLD_TRIGGERED);
        oldCarrier->CastSpell(oldCarrier, SPELL_UNBOUND_PLAGUE_PROTECTION, TRIGGERED_OLD_TRIGGERED);
        professor->CastSpell(newCarrier, SPELL_UNBOUND_PLAGUE_SEARCHER, TRIGGERED_OLD_TRIGGERED);
    }
};

// 70911, 72854, 72855, 72856 - Unbound Plague damage growth
struct PutricideUnboundPlagueDamage : public AuraScript
{
    void OnPeriodicCalculateAmount(Aura* aura, uint32& amount) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        uint32 const baseAmount = uint32(std::max(0, aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0)));
        amount = uint32(float(baseAmount) * std::pow(1.25f, float(aura->GetAuraTicks())));
    }
};

// 72451, 72463, 72671, 72672 - Mutated Plague
struct PutricideMutatedPlague : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0 || !data.spellInfo)
            return;

        Unit* target = aura->GetTarget();
        uint32 const damageSpellId = GetPutricideDifficultySpellId(target, data.spellInfo->Id);
        SpellEntry const* damageInfo = sSpellTemplate.LookupEntry<SpellEntry>(damageSpellId);
        if (!damageInfo)
            return;

        float const multiplier = IsPutricide25Man(target) ? 3.0f : 2.0f;
        uint32 const stacks = aura->GetHolder()->GetStackAmount();
        int32 const baseDamage = damageInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        data.spellInfo = damageInfo;
        data.basePoints[0] = int32(float(baseDamage) * std::pow(multiplier, float(stacks)) * 1.5f);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetEffIndex() != EFFECT_INDEX_0 ||
            aura->GetRemoveMode() == AURA_REMOVE_BY_GAINED_STACK)
            return;

        Unit* target = aura->GetTarget();
        uint32 const healBaseId = uint32(aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0));
        uint32 const healSpellId = GetPutricideDifficultySpellId(target, healBaseId);
        SpellEntry const* healInfo = sSpellTemplate.LookupEntry<SpellEntry>(healSpellId);
        if (!healInfo)
            return;

        int32 const heal = healInfo->CalculateSimpleValue(EFFECT_INDEX_0) * int32(aura->GetHolder()->GetStackAmount());
        target->CastCustomSpell(target, healSpellId, &heal, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED,
            nullptr, aura, aura->GetCasterGuid());
    }
};

// 71770 - Ooze Tank Protection
struct PutricideOozeTankProtection : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        Unit* target = procData.target;
        uint32 const triggerSpell = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
        if (!target || !triggerSpell)
            return SPELL_AURA_PROC_CANT_TRIGGER;

        target->CastSpell(target, triggerSpell, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
        return SPELL_AURA_PROC_CANT_TRIGGER;
    }
};

// 70345 - pause puddle growth while Tear Gas freezes encounter creatures
struct PutricideGrowStacker : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0 && aura->GetTarget()->HasAura(SPELL_TEAR_GAS_CREATURE))
            data.spellInfo = nullptr;
    }
};

// 71278/71279 and difficulty variants - Choking Gas
struct PutricideChokingGasFilter : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*effIdx*/) const override
    {
        return target && !target->IsBoarded();
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
        {
            if (Unit* target = spell->GetUnitTarget())
            {
                target->RemoveAurasDueToSpell(SPELL_OOZE_VARIABLE_OOZE);
                target->RemoveAurasDueToSpell(SPELL_GAS_VARIABLE_GAS);
            }
        }
    }
};

/*######
## npc_growing_ooze_puddle
######*/

struct npc_growing_ooze_puddleAI : public CombatAI
{
    npc_growing_ooze_puddleAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(true);
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }
};

// 70360, 72527 - Eat Ooze
struct EatOoze : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_CLOSEST);
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex effIdx) const override
    {
        // Target 8 is a generic scripted destination-area search in CMaNGOS.
        // Filter it to the retail target entry before selecting the closest
        // result, otherwise the caster or rider can consume the single slot.
        return effIdx != EFFECT_INDEX_0 ||
            (target && target->IsCreature() && target->GetEntry() == NPC_GROWING_OOZE_PUDDLE);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature() || target->GetEntry() != NPC_GROWING_OOZE_PUDDLE)
            return;

        uint32 const triggerSpellId = GetPutricideDifficultySpellId(target,
            spell->m_spellInfo->CalculateSimpleValue(effIdx));
        if (!target->HasAura(triggerSpellId))
            return;

        target->RemoveAuraHolderFromStack(triggerSpellId, 3);

        // despawn if we don't have the grow aura anymore
        if (!target->HasAura(triggerSpellId))
        {
            RemovePutricideDifficultyAura(target, SPELL_GROW_STACKER);
            static_cast<Creature*>(target)->ForcedDespawn(1);
        }
    }
};

// 70539, 72457, 72875, 72876 - Regurgitated Ooze
struct PutricideRegurgitatedOoze : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        instance_icecrown_citadel* instance = caster ?
            static_cast<instance_icecrown_citadel*>(caster->GetInstanceData()) : nullptr;
        if (instance)
            instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NAUSEA, false);
    }
};

/*######
## npc_choking_gas_bomb
######*/

struct npc_choking_gas_bombAI : public CombatAI
{
    npc_choking_gas_bombAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(true);
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }
};

/*######
## npc_puddle_stalker
######*/

struct npc_puddle_stalkerAI : public CombatAI
{
    npc_puddle_stalkerAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(true);
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
    }
};

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
    pNewScript->GetAI = &GetNewAIInstance<boss_professor_putricideAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_volatile_ooze_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_volatile_ooze_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gas_cloud_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_gas_cloud_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_growing_ooze_puddle";
    pNewScript->GetAI = &GetNewAIInstance<npc_growing_ooze_puddleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_choking_gas_bomb";
    pNewScript->GetAI = &GetNewAIInstance<npc_choking_gas_bombAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_puddle_stalker";
    pNewScript->GetAI = &GetNewAIInstance<npc_puddle_stalkerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mutated_abomination";
    pNewScript->GetAI = &GetNewAIInstance<npc_mutated_abominationAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<OozeFloodTrigger>("spell_ooze_flood_trigger");
    RegisterSpellScript<UnstableExperiment>("spell_unstable_experiment");
    RegisterSpellScript<VolatileExperiment>("spell_volatile_experiment");
    RegisterSpellScript<PutricideOozeChannel>("spell_putricide_ooze_channel");
    RegisterSpellScript<PutricideMutationInit>("spell_putricide_mutation_init");
    RegisterSpellScript<PutricideMutatedTransformation>("spell_putricide_mutated_transformation");
    RegisterSpellScript<PutricideMutatedTransformationDismiss>("spell_putricide_mutated_transformation_dismiss");
    RegisterSpellScript<PutricideMutatedTransformationDamage>("spell_putricide_mutated_transformation_damage");
    RegisterSpellScript<PutricideCleanseMutation>("spell_putricide_cleanse_mutation");
    RegisterSpellScript<PutricideClearAuras>("spell_putricide_clear_auras");
    RegisterSpellScript<PutricideChokingGasBomb>("spell_putricide_choking_gas_bomb");
    RegisterSpellScript<PutricideMalleableGoo>("spell_putricide_malleable_goo");
    RegisterSpellScript<PutricideUnboundPlague>("spell_putricide_unbound_plague");
    RegisterSpellScript<PutricideUnboundPlagueDamage>("spell_putricide_unbound_plague_damage");
    RegisterSpellScript<PutricideMutatedPlague>("spell_putricide_mutated_plague");
    RegisterSpellScript<PutricideOozeTankProtection>("spell_putricide_ooze_tank_protection");
    RegisterSpellScript<PutricideGrowStacker>("spell_putricide_grow_stacker");
    RegisterSpellScript<PutricideChokingGasFilter>("spell_putricide_choking_gas_filter");
    RegisterSpellScript<PutricideSlimePuddle>("spell_putricide_slime_puddle");
    RegisterSpellScript<EatOoze>("spell_eat_ooze");
    RegisterSpellScript<PutricideRegurgitatedOoze>("spell_putricide_regurgitated_ooze");
    RegisterSpellScript<OozeFlood>("spell_ooze_flood");
}
