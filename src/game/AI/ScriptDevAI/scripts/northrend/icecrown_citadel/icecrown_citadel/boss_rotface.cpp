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
SDName: boss_rotface
SD%Complete: 90%
SDComment: Some auras require improved stacking handling. Also, Ooze flow triggered spells logic requires core fix.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Spells/SpellAuras.h"

enum
{
    // yells
    SAY_AGGRO                   = -1631071,
    //SAY_SLIME_SPRAY           = -1631072,
    SAY_OOZE_EXPLODE            = -1631073,
    SAY_SLAY_1                  = -1631076,
    SAY_SLAY_2                  = -1631077,
    SAY_BERSERK                 = -1631078,
    SAY_DEATH                   = -1631079,
    SAY_PUTRICIDE_AGGRO         = -1631091,         // used on heroic when Festergut is dead

    EMOTE_SLIME_SPRAY           = -1631202,
    EMOTE_OOZE_GROW_1           = -1631203,         // grow at 2 stacks
    EMOTE_OOZE_GROW_2           = -1631204,         // grow at 3 stacks
    EMOTE_OOZE_GROW_3           = -1631205,         // grow at 4 stacks

    // Mutated Infection
    SPELL_MUTATED_INFECTION_1   = 70090,            // periodic trigger auras; script effect for 69706 which spawns 36897
    SPELL_MUTATED_INFECTION_2   = 70003,
    SPELL_MUTATED_INFECTION_3   = 70004,
    SPELL_MUTATED_INFECTION_4   = 70005,
    SPELL_MUTATED_INFECTION_5   = 70006,

    // Slime Spray
    SPELL_SLIME_SPRAY_SUMMON    = 70882,            // spawns 37986
    SPELL_FACE_SPRAY_STALKER    = 70881,            // targets 37986
    SPELL_SLIME_SPRAY           = 69508,            // targets 37986

    // Ooze Flood
    SPELL_OOZE_FLOOD_PERIODIC   = 70069,            // periodically trigger 69795
    SPELL_OOZE_FLOOD_REMOVE     = 70079,            // cancel aura 70069

    // Little Ooze spells
    SPELL_GREEN_ABOMINATION_HIT = 70001,
    SPELL_WEAK_RADIATING_OOZE   = 69750,
    SPELL_LITTLE_OOZE_COMBINE   = 69537,            // periodic check for 69538; combines two small oozes
    SPELL_STICKY_OOZE           = 69774,            // spawns 37006
    //SPELL_MERGE               = 69889,            // summons 36899
    //SPELL_UNSTABLE_OOZE_TRIGG = 69644,            // increate stacks of 69558 on big ooze

    // Sticky Ooze
    SPELL_STICKY_AURA           = 69776,            // damage aura on npc 37006

    // Big Ooze spells
    SPELL_UNSTABLE_OOZE         = 69558,            // stacking buff; triggers 69839 at 5 stacks
    SPELL_RADIATING_OOZE        = 69760,
    SPELL_BIG_OOZE_COMBINE      = 69552,            // periodic check for 69553; combines a small ooze with a large one
    SPELL_BIG_OOZE_BUFF_COMB    = 69611,            // periodic check to combine 69558; possible triggers 69617; combines two large oozes
    SPELL_UNSTABLE_EXPLOSION    = 69839,            // prepare the explosion triggers; summons creature 38107 on each target
    SPELL_OOZE_EXPLOSION        = 69832,

    // heroic spells
    SPELL_VILE_TRIGGER          = 72285,            // triggers 72287 which spawns 38548; Putricide casts 72272 on the spawned npc
    SPELL_VILE_GAS              = 72272,

    // npcs
    NPC_LITTLE_OOZE             = 36897,
    NPC_STICKY_OOZE             = 37006,
    NPC_BIG_OOZE                = 36899,
    NPC_VILE_GAS_STALKER        = 38548,
    NPC_OOZE_SPRAY_STALKER      = 37986,
    NPC_OOZE_EXPLOSION_STALKER  = 38107,

    // other
    MAX_MUTATE_INFACTION_STEPS  = 5,
};

static const uint32 uiMutatedInfections[MAX_MUTATE_INFACTION_STEPS] =
{
    SPELL_MUTATED_INFECTION_1,
    SPELL_MUTATED_INFECTION_2,
    SPELL_MUTATED_INFECTION_3,
    SPELL_MUTATED_INFECTION_4,
    SPELL_MUTATED_INFECTION_5
};

static const float afBalconyLocation[3] = {4390.371f, 3164.5f, 389.389f};

/*######
## boss_rotface
######*/

struct boss_rotfaceAI : public ScriptedAI
{
    boss_rotfaceAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiSlimeSprayTimer;
    uint32 m_uiMutatedInfectionTimer;
    uint32 m_uiInfectionsRate;
    uint32 m_uiVileGasTimer;

    void Reset() override
    {
        m_uiSlimeSprayTimer             = 5000;
        m_uiMutatedInfectionTimer       = 60000;
        m_uiVileGasTimer                = 30000;
        m_uiInfectionsRate              = 1;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ROTFACE, IN_PROGRESS);

            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
            {
                pPutricide->CastSpell(pPutricide, SPELL_OOZE_FLOOD_PERIODIC, TRIGGERED_OLD_TRIGGERED);
                pPutricide->SetWalk(false);
                pPutricide->GetMotionMaster()->MovePoint(102, afBalconyLocation[0], afBalconyLocation[1], afBalconyLocation[2]);

                // heroic aggro text
                if (m_pInstance->IsHeroicDifficulty() && m_pInstance->GetData(TYPE_FESTERGUT) == DONE)
                    DoScriptText(SAY_PUTRICIDE_AGGRO, pPutricide);
            }
        }

        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_MUTATED_INFECTION_1, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_REMOVE, CAST_TRIGGERED);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ROTFACE, FAIL);

            // reset putricide
            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->AI()->EnterEvadeMode();
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature, pVictim);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_REMOVE, CAST_TRIGGERED);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ROTFACE, DONE);

            // reset putricide
            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->AI()->EnterEvadeMode();
        }

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_OOZE_SPRAY_STALKER)
        {
            DoScriptText(EMOTE_SLIME_SPRAY, m_creature);
            DoCastSpellIfCan(pSummoned, SPELL_SLIME_SPRAY, CAST_TRIGGERED);
            DoCastSpellIfCan(pSummoned, SPELL_FACE_SPRAY_STALKER, CAST_TRIGGERED);
        }
        else if (pSummoned->GetEntry() == NPC_VILE_GAS_STALKER)
        {
            if (!m_pInstance)
                return;

            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->CastSpell(pSummoned, SPELL_VILE_GAS, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_pInstance)
            return;

        // Slime Spray
        if (m_uiSlimeSprayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SLIME_SPRAY_SUMMON) == CAST_OK)
                m_uiSlimeSprayTimer = urand(20000, 25000);
        }
        else
            m_uiSlimeSprayTimer -= uiDiff;

        // Mutated Infection - faster with time
        // implemented this instead of phases
        if (m_uiInfectionsRate < MAX_MUTATE_INFACTION_STEPS)
        {
            if (m_uiMutatedInfectionTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, uiMutatedInfections[m_uiInfectionsRate], CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->RemoveAurasDueToSpell(uiMutatedInfections[m_uiInfectionsRate - 1]);
                    // every next 15 seconds faster
                    m_uiMutatedInfectionTimer = 60000 - m_uiInfectionsRate * 15000;
                    ++m_uiInfectionsRate;
                }
            }
            else
                m_uiMutatedInfectionTimer -= uiDiff;
        }

        // Heroic spells
        if (m_pInstance->IsHeroicDifficulty())
        {
            if (m_uiVileGasTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VILE_TRIGGER) == CAST_OK)
                    m_uiVileGasTimer = 30000;
            }
            else
                m_uiVileGasTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_rotface(Creature* pCreature)
{
    return new boss_rotfaceAI(pCreature);
}

/*######
## mob_little_ooze
######*/

struct mob_little_oozeAI : public ScriptedAI
{
    mob_little_oozeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiStickyOozeTimer;

    void Reset() override
    {
        m_uiStickyOozeTimer = 5000;

        DoCastSpellIfCan(m_creature, SPELL_GREEN_ABOMINATION_HIT, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_WEAK_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_LITTLE_OOZE_COMBINE, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_STICKY_OOZE)
            pSummoned->CastSpell(pSummoned, SPELL_STICKY_AURA, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiStickyOozeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STICKY_OOZE) == CAST_OK)
                m_uiStickyOozeTimer = 15000;
        }
        else
            m_uiStickyOozeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_little_ooze(Creature* pCreature)
{
    return new mob_little_oozeAI(pCreature);
}

/*######
## mob_big_ooze
######*/

struct mob_big_oozeAI : public ScriptedAI
{
    mob_big_oozeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiStickyOozeTimer;
    uint32 m_uiUnstableExplosionTimer;

    void Reset() override
    {
        m_uiStickyOozeTimer             = 5000;
        m_uiUnstableExplosionTimer      = 0;

        DoCastSpellIfCan(m_creature, SPELL_GREEN_ABOMINATION_HIT, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_COMBINE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_BUFF_COMB, CAST_TRIGGERED);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // prepare explosion; Rotface yells and achiev is failed
            if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPLOSION) == CAST_OK)
            {
                if (m_pInstance)
                {
                    if (Creature* pRotface = m_pInstance->GetSingleCreatureFromStorage(NPC_ROTFACE))
                        DoScriptText(SAY_OOZE_EXPLODE, pRotface);

                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DANCES_OOZES, false);
                }

                m_uiUnstableExplosionTimer = 5000;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_STICKY_OOZE)
            pSummoned->CastSpell(pSummoned, SPELL_STICKY_AURA, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Unstable Ooze
        if (m_uiUnstableExplosionTimer)
        {
            if (m_uiUnstableExplosionTimer <= uiDiff)
            {
                CreatureList lTargetsInRange;
                GetCreatureListWithEntryInGrid(lTargetsInRange, m_creature, NPC_OOZE_EXPLOSION_STALKER, DEFAULT_VISIBILITY_INSTANCE);

                if (lTargetsInRange.empty())
                {
                    script_error_log("Instance Icecrown Citadel: ERROR Failed to find creature %u for Unstable Ooze explosion.", NPC_OOZE_EXPLOSION_STALKER);
                    m_uiUnstableExplosionTimer = 0;
                    return;
                }

                // explode all the summoned triggers
                for (CreatureList::const_iterator itr = lTargetsInRange.begin(); itr != lTargetsInRange.end(); ++itr)
                {
                    DoCastSpellIfCan(*itr, SPELL_OOZE_EXPLOSION, CAST_TRIGGERED);
                    (*itr)->ForcedDespawn(10000);
                }

                m_uiUnstableExplosionTimer = 0;
            }
            else
                m_uiUnstableExplosionTimer -= uiDiff;
        }

        // Sticky Ooze
        if (m_uiStickyOozeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STICKY_OOZE) == CAST_OK)
                m_uiStickyOozeTimer = 15000;
        }
        else
            m_uiStickyOozeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_big_ooze(Creature* pCreature)
{
    return new mob_big_oozeAI(pCreature);
}

bool EffectScriptEffectCreature_spell_unstable_ooze(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_UNSTABLE_OOZE && uiEffIndex == EFFECT_INDEX_2 && pCreatureTarget->GetEntry() == NPC_BIG_OOZE)
    {
        // send AI event on 5 stacks of unstable Ooze
        SpellAuraHolder* pOozeHolder = pCreatureTarget->GetSpellAuraHolder(uiSpellId);
        if (pOozeHolder)
        {
            // Note: stacks are increased after the effect is processed, so we need to use (stacks - 1)
            switch (pOozeHolder->GetStackAmount())
            {
                case 1:
                    DoScriptText(EMOTE_OOZE_GROW_1, pCreatureTarget);
                    break;
                case 2:
                    DoScriptText(EMOTE_OOZE_GROW_2, pCreatureTarget);
                    break;
                case 4:
                    pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
                // no break;
                case 3:
                    DoScriptText(EMOTE_OOZE_GROW_3, pCreatureTarget);
                    break;
            }
        }

        return true;
    }

    return false;
}

void AddSC_boss_rotface()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_rotface";
    pNewScript->GetAI = &GetAI_boss_rotface;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_little_ooze";
    pNewScript->GetAI = &GetAI_mob_little_ooze;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_big_ooze";
    pNewScript->GetAI = &GetAI_mob_big_ooze;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_unstable_ooze;
    pNewScript->RegisterSelf();
}
