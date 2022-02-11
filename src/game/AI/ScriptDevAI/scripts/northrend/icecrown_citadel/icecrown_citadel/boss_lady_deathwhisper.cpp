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
SDName: boss_lady_deathwhisper
SD%Complete: 100%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    // yells
    SAY_AGGRO                   = -1631018,
    SAY_PHASE_TWO               = -1631019,
    SAY_DARK_EMPOWERMENT        = -1631020,
    SAY_DARK_TRANSFORMATION     = -1631021,
    SAY_ANIMATE_DEAD            = -1631022,
    SAY_DOMINATE_MIND           = -1631023,
    SAY_BERSERK                 = -1631024,
    SAY_DEATH                   = -1631025,
    SAY_SLAY_1                  = -1631026,
    SAY_SLAY_2                  = -1631027,

    // spells - phase 1
    SPELL_SHADOW_CHANNELING     = 43897,
    SPELL_MANA_BARRIER          = 70842,
    SPELL_SHADOW_BOLT           = 71254,

    // phase 2
    SPELL_INSIGNIFICANCE        = 71204,
    SPELL_FROSTBOLT             = 71420,
    SPELL_FROSTBOLT_VOLLEY      = 72905,
    SPELL_SUMMON_SPIRIT         = 71363,            // triggers 71426

    // common
    SPELL_BERSERK               = 26662,
    SPELL_DOMINATE_MIND         = 71289,
    SPELL_DEATH_AND_DECAY       = 71001,
    SPELL_DARK_EMPOWERMENT      = 70896,            // dummy - triggers 70901 - only on Adherents - transforms target into 38136
    SPELL_DARK_TRANSFORMATION   = 70895,            // dummy - triggers 70900 - only on Fanatics - transforms target into 38135
    SPELL_DARK_MARTYRDOM        = 70897,            // dummy - triggers 70903 on Adherents or 71236 on Fanatics
    // SPELL_SUMMON_ADHERENT     = 70820,            // cast by the stalkers - only server side
    // SPELL_SUMMON_FANATIC      = 70819,            // cast by the stalkers - only server side

    // phase 2 npcs
    NPC_VENGEFUL_SHADE          = 38222,            // has aura 71494
};

enum DeathwhisperActions
{
    DEATHWHISPER_BERSRK,
    DEATHWHISPER_DOMINATE_MIND,
    DEATHWHISPER_DEATH_AND_DECAY,

    DEATHWHISPER_SHADOW_BOLT,
    DEATHWHISPER_CULTIST_SUMMON,
    DEATHWHISPER_CULTIST_BUFF,
    DEATHWHISPER_CULTIST_MARTYRDOM,

    DEATHWHISPER_INSIGNIFICANCE,
    DEATHWHISPER_FROSTBOLT,
    DEATHWHISPER_FROSTBOLT_VOLLEY,
    DEATHWHISPER_SUMMON_SPIRIT,

    DEATHWHISPER_ACTION_MAX
};

static const uint32 aLeftSummonedCultists[3] = {NPC_CULT_ADHERENT, NPC_CULT_FANATIC, NPC_CULT_ADHERENT};
static const uint32 aRightSummonedCultists[3] = {NPC_CULT_FANATIC, NPC_CULT_ADHERENT, NPC_CULT_FANATIC};

struct boss_lady_deathwhisperAI : public CombatAI
{
    boss_lady_deathwhisperAI(Creature* creature) : CombatAI(creature, DEATHWHISPER_ACTION_MAX), m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        // Set the max allowed mind control targets
        if (m_instance)
        {
            m_bIsHeroicMode = m_instance->IsHeroicDifficulty();

            if (m_instance->Is25ManDifficulty())
                m_uiMindControlCount = m_instance->IsHeroicDifficulty() ? 3 : 1;
            else
                m_uiMindControlCount = m_instance->IsHeroicDifficulty() ? 1 : 0;
        }

        // common actions
        AddCombatAction(DEATHWHISPER_BERSRK, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(DEATHWHISPER_DEATH_AND_DECAY, 20000u);
        AddCombatAction(DEATHWHISPER_DOMINATE_MIND, 30000u);

        // phase 1 actions
        AddCombatAction(DEATHWHISPER_SHADOW_BOLT, 0u);
        AddCombatAction(DEATHWHISPER_CULTIST_SUMMON, m_bIsHeroicMode ? 5000u : 10000u);
        AddCombatAction(DEATHWHISPER_CULTIST_BUFF, true);
        AddCombatAction(DEATHWHISPER_CULTIST_MARTYRDOM, true);

        // phase 2 actions
        AddCombatAction(DEATHWHISPER_INSIGNIFICANCE, true);
        AddCombatAction(DEATHWHISPER_FROSTBOLT, true);
        AddCombatAction(DEATHWHISPER_FROSTBOLT_VOLLEY, true);
        AddCombatAction(DEATHWHISPER_SUMMON_SPIRIT, true);

        SetMeleeEnabled(false);
        SetCombatMovement(false);
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_CHANNELING);

        Reset();
    }

    instance_icecrown_citadel* m_instance;

    bool m_bIsHeroicMode;
    bool m_bIsLeftSideSummon;
    bool m_bIsPhaseOne;

    uint8 m_uiMindControlCount;

    GuidList m_lCultistSpawnedGuidList;
    GuidVector m_vRightStalkersGuidVector;
    GuidVector m_vLeftStalkersGuidVector;
    ObjectGuid m_middleStalkerGuid;

    void Reset() override
    {
        CombatAI::Reset();

        m_bIsPhaseOne = true;
        m_bIsLeftSideSummon = roll_chance_i(50);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_MANA_BARRIER, CAST_TRIGGERED);

        if (m_instance)
        {
            m_instance->SetData(TYPE_LADY_DEATHWHISPER, IN_PROGRESS);

            // Sort the summoning stalkers
            GuidList lStalkersGuidList;
            m_instance->GetDeathwhisperStalkersList(lStalkersGuidList);
            DoSortSummoningStalkers(lStalkersGuidList);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_LADY_DEATHWHISPER, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_LADY_DEATHWHISPER, FAIL);

        SetMeleeEnabled(false);
        SetCombatMovement(false);
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_CHANNELING);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CULT_ADHERENT:
            case NPC_CULT_FANATIC:
                m_lCultistSpawnedGuidList.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_VENGEFUL_SHADE:
                pSummoned->SetForceAttackingCapability(true);
                break;
        }

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() != NPC_VENGEFUL_SHADE)
            m_lCultistSpawnedGuidList.remove(pSummoned->GetObjectGuid());
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoStartSecondPhase();
    }

    // Wrapper to help sort the summoning stalkers
    void DoSortSummoningStalkers(GuidList& lDeathwhisperStalkers)
    {
        CreatureList lRightStalkers;
        CreatureList lLeftStalkers;

        if (!lDeathwhisperStalkers.empty())
        {
            for (const auto& guid : lDeathwhisperStalkers)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(guid))
                {
                    if (pStalker->GetPositionZ() > 60.0f)
                        m_middleStalkerGuid = pStalker->GetObjectGuid();
                    else if (pStalker->GetPositionY() < 2215.0f)
                        lLeftStalkers.push_back(pStalker);
                    else
                        lRightStalkers.push_back(pStalker);
                }
            }

            lLeftStalkers.sort(sortFromNorthToSouth);
            lRightStalkers.sort(sortFromNorthToSouth);

            // Store the sorted stalkers in a vector for each side
            for (CreatureList::const_iterator itr = lLeftStalkers.begin(); itr != lLeftStalkers.end(); ++itr)
                m_vLeftStalkersGuidVector.push_back((*itr)->GetObjectGuid());
            for (CreatureList::const_iterator itr = lRightStalkers.begin(); itr != lRightStalkers.end(); ++itr)
                m_vRightStalkersGuidVector.push_back((*itr)->GetObjectGuid());
        }
        else
            script_error_log("instance_icecrown_citadel: Error: cannot find creature %u in instance", NPC_DEATHWHISPER_SPAWN_STALKER);
    }

    static bool sortFromNorthToSouth(Creature* pFirst, Creature* pSecond)
    {
        return pFirst && pSecond && pFirst->GetPositionX() < pSecond->GetPositionX();
    }

    // Wrapper to select a random cultist
    Creature* DoSelectRandomCultist(uint32 uiEntry = 0)
    {
        std::vector<Creature*> vCultists;
        vCultists.reserve(m_lCultistSpawnedGuidList.size());

        for (const auto& guid : m_lCultistSpawnedGuidList)
        {
            if (Creature* pCultist = m_creature->GetMap()->GetCreature(guid))
            {
                // Allow to be sorted them by entry
                if (!uiEntry)
                    vCultists.push_back(pCultist);
                else if (pCultist->GetEntry() == uiEntry)
                    vCultists.push_back(pCultist);
            }
        }

        if (vCultists.empty())
            return nullptr;

        return vCultists[urand(0, vCultists.size() - 1)];
    }

    // Wrapper to handle the adds summmoning
    void DoSummonCultistWave()
    {
        if (!m_instance)
            return;

        // failsafe check
        if (m_vLeftStalkersGuidVector.size() < 3 || m_vRightStalkersGuidVector.size() < 3)
        {
            script_error_log("instance_icecrown_citadel: Error: Cannot find all left or right spawn stalkers.");
            return;
        }

        // On 25 man mode we need to summon on all points
        if (m_instance->Is25ManDifficulty())
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_vLeftStalkersGuidVector[i]))
                    m_creature->SummonCreature(aLeftSummonedCultists[i], pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_vRightStalkersGuidVector[i]))
                    m_creature->SummonCreature(aRightSummonedCultists[i], pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
            }

            if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_middleStalkerGuid))
                m_creature->SummonCreature(roll_chance_i(50) ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT, pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ(), 0.0f, TEMPSPAWN_CORPSE_DESPAWN, 0);
        }
        // On 10 man mode we summon on the left or on the right
        else
        {
            // Summon just 1 add in phase 2 heroic
            if (m_bIsHeroicMode && !m_bIsPhaseOne)
            {
                if (Creature* pStalker = m_creature->GetMap()->GetCreature(m_middleStalkerGuid))
                    m_creature->SummonCreature(roll_chance_i(50) ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT, pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ(), 0.0f, TEMPSPAWN_CORPSE_DESPAWN, 0);
            }
            else
            {
                GuidVector vTempVector = m_bIsLeftSideSummon ? m_vLeftStalkersGuidVector : m_vRightStalkersGuidVector;
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (Creature* pStalker = m_creature->GetMap()->GetCreature(vTempVector[i]))
                        m_creature->SummonCreature(m_bIsLeftSideSummon ? aLeftSummonedCultists[i] : aRightSummonedCultists[i], pStalker->GetPositionX(), pStalker->GetPositionY(), pStalker->GetPositionZ(), 0, TEMPSPAWN_DEAD_DESPAWN, 0);
                }

                // change sides for next summoning
                m_bIsLeftSideSummon = !m_bIsLeftSideSummon;
            }
        }
    }

    // Wrapper to handle the second phase start
    void DoStartSecondPhase()
    {
        DoScriptText(SAY_PHASE_TWO, m_creature);
        SetCombatMovement(true);
        SetMeleeEnabled(true);

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

        // stop the summon on normal mode
        if (!m_bIsHeroicMode)
            DisableCombatAction(DEATHWHISPER_CULTIST_SUMMON);

        DisableCombatAction(DEATHWHISPER_SHADOW_BOLT);

        ResetCombatAction(DEATHWHISPER_INSIGNIFICANCE, 1000);
        ResetCombatAction(DEATHWHISPER_FROSTBOLT, 5000);
        ResetCombatAction(DEATHWHISPER_FROSTBOLT_VOLLEY, 20000);
        ResetCombatAction(DEATHWHISPER_SUMMON_SPIRIT, 20000);

        m_bIsPhaseOne = false;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DEATHWHISPER_BERSRK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case DEATHWHISPER_DEATH_AND_DECAY:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
                    if (DoCastSpellIfCan(target, SPELL_DEATH_AND_DECAY) == CAST_OK)
                        ResetCombatAction(action, 20000);
                }
                break;
            case DEATHWHISPER_DOMINATE_MIND:
                for (uint8 i = 0; i < m_uiMindControlCount; ++i)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_DOMINATE_MIND, SELECT_FLAG_PLAYER))
                        DoCastSpellIfCan(target, SPELL_DOMINATE_MIND, CAST_TRIGGERED);
                }

                DoScriptText(SAY_DOMINATE_MIND, m_creature);
                ResetCombatAction(action, 45000);
                break;
            case DEATHWHISPER_SHADOW_BOLT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_SHADOW_BOLT) == CAST_OK)
                        ResetCombatAction(action, urand(2000, 3000));
                }
                break;
            case DEATHWHISPER_CULTIST_SUMMON:
                DoSummonCultistWave();
                ResetCombatAction(DEATHWHISPER_CULTIST_BUFF, 10000);
                ResetCombatAction(DEATHWHISPER_CULTIST_MARTYRDOM, 40000);
                ResetCombatAction(action, m_bIsHeroicMode ? 45000 : 60000);
                break;
            case DEATHWHISPER_CULTIST_BUFF:
            {
                // Choose a random of Fanatic or Adherent
                bool bIsFanatic = roll_chance_i(50);
                uint32 uiNpcEntry = bIsFanatic ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT;
                uint32 uiSpellEntry = bIsFanatic ? SPELL_DARK_TRANSFORMATION : SPELL_DARK_EMPOWERMENT;
                int32 iTextEntry = bIsFanatic ? SAY_DARK_TRANSFORMATION : SAY_DARK_EMPOWERMENT;

                // Try to select a random Cultist of entry
                if (Creature* target = DoSelectRandomCultist(uiNpcEntry))
                {
                    if (DoCastSpellIfCan(target, uiSpellEntry) == CAST_OK)
                    {
                        // Remove the selected cultist from the list because we don't want it selected twice
                        m_lCultistSpawnedGuidList.remove(target->GetObjectGuid());
                        DoScriptText(iTextEntry, m_creature);
                        DisableCombatAction(action);
                    }
                }

                break;
            }
            case DEATHWHISPER_CULTIST_MARTYRDOM:
                // Try to get a target on which to cast Martyrdom
                if (Creature* target = DoSelectRandomCultist())
                {
                    if (DoCastSpellIfCan(target, SPELL_DARK_MARTYRDOM) == CAST_OK)
                    {
                        DoScriptText(SAY_ANIMATE_DEAD, m_creature);
                        DisableCombatAction(action);
                    }
                }
                break;
            case DEATHWHISPER_INSIGNIFICANCE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_INSIGNIFICANCE) == CAST_OK)
                    ResetCombatAction(action, 7000);
                break;
            case DEATHWHISPER_FROSTBOLT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_FROSTBOLT) == CAST_OK)
                        ResetCombatAction(action, 12000);
                }
                break;
            case DEATHWHISPER_FROSTBOLT_VOLLEY:
                if (DoCastSpellIfCan(m_creature, SPELL_FROSTBOLT_VOLLEY) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            case DEATHWHISPER_SUMMON_SPIRIT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_SUMMON_SPIRIT) == CAST_OK)
                        ResetCombatAction(action, 10000);
                }
                break;
        }
    }
};

/*######
## spell_mana_barrier_aura - 70842
######*/

struct spell_mana_barrier_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        uint32 damage = target->GetMaxHealth() - target->GetHealth();
        if (!damage)
            return;

        if (target->GetPower(POWER_MANA) < damage)
        {
            damage = target->GetPower(POWER_MANA);
            target->RemoveAurasDueToSpell(aura->GetId());

            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
        }

        target->DealHeal(target, damage, aura->GetSpellProto());
        target->ModifyPower(POWER_MANA, -int32(damage));
    }
};

/*######
## spell_dark_transformation - 70895
######*/

struct spell_dark_transformation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->InterruptNonMeleeSpells(false);
        target->CastSpell(target, 70900, TRIGGERED_NONE);
    }
};

/*######
## spell_dark_empowerment - 70896
######*/

struct spell_dark_empowerment : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->InterruptNonMeleeSpells(false);
        target->CastSpell(target, 70901, TRIGGERED_NONE);
    }
};

/*######
## spell_dark_martyrdom - 70897
######*/

struct spell_dark_martyrdom : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->InterruptNonMeleeSpells(false);

        if (target->GetEntry() == NPC_CULT_FANATIC)
            target->CastSpell(target, 71236, TRIGGERED_NONE);
        else if (target->GetEntry() == NPC_CULT_ADHERENT)
            target->CastSpell(target, 70903, TRIGGERED_NONE);
    }
};

/*######
## spell_dark_transformation_aura - 70900
######*/

struct spell_dark_transformation_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        if (apply)
        {
            target->CastSpell(target, 17683, TRIGGERED_OLD_TRIGGERED);

            // update entry to creature 38135
            uint32 creatureEntry = aura->GetSpellProto()->EffectMiscValue[EFFECT_INDEX_2];
            Creature* fanatic = static_cast<Creature*>(target);

            fanatic->UpdateEntry(creatureEntry);
            fanatic->AIM_Initialize();

            if (fanatic->GetVictim())
                fanatic->GetMotionMaster()->MoveChase(fanatic->GetVictim());
        }
    }
};

/*######
## spell_dark_empowerment_aura - 70901
######*/

struct spell_dark_empowerment_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        if (apply)
        {
            target->CastSpell(target, 17683, TRIGGERED_OLD_TRIGGERED);

            // update entry to creature 38136
            Creature* adherent = static_cast<Creature*>(target);

            adherent->UpdateEntry(38136);
            adherent->AIM_Initialize();
        }
    }
};

/*######
## spell_dark_fanatic_martyrdom_aura - 71236, 72495, 72496, 72497
######*/

struct spell_dark_fanatic_martyrdom_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        if (apply)
        {
            target->CastSpell(target, 17683, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 71235, TRIGGERED_OLD_TRIGGERED);

            // update entry to creature 38009
            uint32 creatureEntry = aura->GetSpellProto()->EffectMiscValue[EFFECT_INDEX_1];
            Creature* fanatic = static_cast<Creature*>(target);

            fanatic->UpdateEntry(creatureEntry);
            fanatic->AIM_Initialize();

            if (fanatic->GetVictim())
                fanatic->GetMotionMaster()->MoveChase(fanatic->GetVictim());
        }
    }
};

/*######
## spell_dark_adherent_martyrdom_aura - 70903, 72498, 72499, 72500
######*/

struct spell_dark_adherent_martyrdom_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        if (apply)
        {
            target->CastSpell(target, 17683, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, 71234, TRIGGERED_OLD_TRIGGERED);

            // update entry to creature 38010
            uint32 creatureEntry = aura->GetSpellProto()->EffectMiscValue[EFFECT_INDEX_1];
            Creature* adherent = static_cast<Creature*>(target);

            adherent->UpdateEntry(creatureEntry);
            adherent->AIM_Initialize();
        }
    }
};

void AddSC_boss_lady_deathwhisper()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lady_deathwhisper";
    pNewScript->GetAI = &GetNewAIInstance<boss_lady_deathwhisperAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_mana_barrier_aura>("spell_mana_barrier_aura");
    RegisterSpellScript<spell_dark_transformation>("spell_dark_transformation");
    RegisterSpellScript<spell_dark_empowerment>("spell_dark_empowerment");
    RegisterSpellScript<spell_dark_martyrdom>("spell_dark_martyrdom");
    RegisterSpellScript<spell_dark_transformation_aura>("spell_dark_transformation_aura");
    RegisterSpellScript<spell_dark_empowerment_aura>("spell_dark_empowerment_aura");
    RegisterSpellScript<spell_dark_fanatic_martyrdom_aura>("spell_dark_fanatic_martyrdom_aura");
    RegisterSpellScript<spell_dark_adherent_martyrdom_aura>("spell_dark_adherent_martyrdom_aura");
}
