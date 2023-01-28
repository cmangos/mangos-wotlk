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
SDName: Boss_Noth
SD%Complete: 80
SDComment: Summons need verify, need better phase-switch support (unattackable?)
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    SAY_AGGRO1                          = 13061,
    SAY_AGGRO2                          = 13062,
    SAY_AGGRO3                          = 13063,
    SAY_SUMMON                          = 13067,
    SAY_SLAY1                           = 13065,
    SAY_SLAY2                           = 13066,
    SAY_DEATH                           = 13064,

    EMOTE_WARRIOR                       = 32974,
    EMOTE_SKELETON                      = 32977,
    EMOTE_TELEPORT                      = 32331,
    EMOTE_TELEPORT_RETURN               = 32976,

    SPELL_TELEPORT                      = 29216,
    SPELL_TELEPORT_RETURN               = 29231,

    SPELL_BLINK_1                       = 29208,
    SPELL_BLINK_2                       = 29209,
    SPELL_BLINK_3                       = 29210,
    SPELL_BLINK_4                       = 29211,

    SPELL_CRIPPLE                       = 29212,
    SPELL_CRIPPLE_H                     = 54814,
    SPELL_CURSE_PLAGUEBRINGER           = 29213,
    SPELL_CURSE_PLAGUEBRINGER_H         = 54835,

    SPELL_BERSERK                       = 26662,            // guesswork, but very common berserk spell in naxx

    SPELL_SUMMON_WARRIOR_1              = 29247,
    SPELL_SUMMON_WARRIOR_2              = 29248,
    SPELL_SUMMON_WARRIOR_3              = 29249,

    SPELL_SUMMON_WARRIOR_THREE          = 29237,

    SPELL_SUMMON_CHAMP01                = 29217,
    SPELL_SUMMON_CHAMP02                = 29224,
    SPELL_SUMMON_CHAMP03                = 29225,
    SPELL_SUMMON_CHAMP04                = 29227,
    SPELL_SUMMON_CHAMP05                = 29238,
    SPELL_SUMMON_CHAMP06                = 29255,
    SPELL_SUMMON_CHAMP07                = 29257,
    SPELL_SUMMON_CHAMP08                = 29258,
    SPELL_SUMMON_CHAMP09                = 29262,
    SPELL_SUMMON_CHAMP10                = 29267,

    SPELL_SUMMON_GUARD01                = 29226,
    SPELL_SUMMON_GUARD02                = 29239,
    SPELL_SUMMON_GUARD03                = 29256,
    SPELL_SUMMON_GUARD04                = 29268,

    SPELL_SUMMON_SKELETONS              = 29252,            // Periodic trigger aura that controls spawning of adds

    PHASE_GROUND                        = 0,
    PHASE_BALCONY                       = 1,

    PHASE_SKELETON_1                    = 1,
    PHASE_SKELETON_2                    = 2,
    PHASE_SKELETON_3                    = 3
};

enum NothAction
{
    NOTH_PHASE_BALCONY,
    NOTH_PHASE_GROUND,
    NOTH_ACTIONS_MAX,
    SUMMONED_UNROOT,
};

struct boss_nothAI : public BossAI
{
    boss_nothAI(Creature* creature) : BossAI(creature, NOTH_ACTIONS_MAX),
        m_instance(static_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_NOTH);
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
        AddOnDeathText(SAY_DEATH);
        AddOnAggroText(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3);
        AddCombatAction(NOTH_PHASE_BALCONY, 90s);
        AddCombatAction(NOTH_PHASE_GROUND, true);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;
    GuidVector m_undeadSummonGuidList;

    uint8 m_uiPhase, m_uiPhaseSub;

    void Reset() override
    {
        m_uiPhase = PHASE_GROUND;
        m_uiPhaseSub = PHASE_GROUND;

        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        SetReactState(REACT_AGGRESSIVE);
        SetAIImmobilizedState(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void EnterEvadeMode() override
    {
        m_creature->SetRespawnDelay(10, true);
        m_creature->ForcedDespawn();

        DespawnGuids(m_undeadSummonGuidList);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->AI()->AddCustomAction(SUMMONED_UNROOT, 3s + 500ms, [summoned]()
        {
            if (summoned && summoned->AI())
                summoned->AI()->SetAIImmobilizedState(false);
        });
        summoned->AI()->SetAIImmobilizedState(true);
        summoned->SetInCombatWithZone();
        m_undeadSummonGuidList.push_back(summoned->GetObjectGuid());
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        if (caster == m_creature && spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEAP)
            DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_CRIPPLE : SPELL_CRIPPLE_H);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        static std::set<uint32> const spellBlinkSet =
        {
            SPELL_BLINK_1, SPELL_BLINK_2, SPELL_BLINK_3, SPELL_BLINK_4
        };
        if (spellBlinkSet.find(spellInfo->Id) != spellBlinkSet.end())
            DoResetThreat();
    }

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);
        DoCastSpellIfCan(nullptr, SPELL_SUMMON_SKELETONS, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case NOTH_PHASE_GROUND:
            {
                return 110s;
            }
            case NOTH_PHASE_BALCONY:
            {
                return 70s;
            }
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NOTH_PHASE_BALCONY:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_TELEPORT) == CAST_OK)
                {
                    static const std::vector<uint32> GROUND_PHASE_ACTIONS = {NOTH_PHASE_BALCONY};
                    for (uint32 action : GROUND_PHASE_ACTIONS)
                        DisableCombatAction(action);

                    DoBroadcastText(EMOTE_TELEPORT, m_creature);
                    SetAIImmobilizedState(true);
                    m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    SetMeleeEnabled(false);
                    StopTargeting(true);
                    m_creature->SetTarget(nullptr);
                    ++m_uiPhaseSub;
                    m_uiPhase = PHASE_BALCONY;

                    ResetCombatAction(NOTH_PHASE_GROUND, GetSubsequentActionTimer(NOTH_PHASE_GROUND));
                }
                return;
            }
            case NOTH_PHASE_GROUND:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_TELEPORT_RETURN) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_TELEPORT_RETURN, m_creature);
                    SetAIImmobilizedState(false);
                    m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    SetMeleeEnabled(true);
                    HandleTargetRestoration();
                    m_uiPhase = PHASE_GROUND;
                    ResetCombatAction(NOTH_PHASE_BALCONY, GetSubsequentActionTimer(NOTH_PHASE_BALCONY));
                    DisableCombatAction(action);
                    AddInitialCooldowns();
                }
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

// 29252 - Summon Skeletons - Only using one spell - there are multiple but none are seen in sniffs so taking one to guess it
struct SummonSkeletonsNoth : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.spellInfo = nullptr; // disable normal triggering
        Unit* target = aura->GetTarget();
        bool regularMode = target->GetMap()->IsRegularDifficulty();

        // all spells confirmed to be triggered from aura
        switch (aura->GetAuraTicks()) // tick every 5 seconds
        {
            default: break;
            case 3: // 15s
            case 9: // 45s
            case 15: // 75s
            case 21: // 105s
            case 39: // 195s
            case 45: // 225s
            case 51: // 255s
            case 57: // 285s
            case 75: // 375s
            case 81: // 405s
            case 87: // 435s
            case 93: // 465s
            {
                DoBroadcastText(SAY_SUMMON, target);
                DoBroadcastText(EMOTE_WARRIOR, target);

                if (regularMode)
                {
                    std::vector<uint32> spellSummonPlaguedWarrior =
                    {
                        SPELL_SUMMON_WARRIOR_1, SPELL_SUMMON_WARRIOR_2, SPELL_SUMMON_WARRIOR_3
                    };

                    for (uint8 i = 0; i < 2; ++i)
                    {
                        uint32 index = urand(0, spellSummonPlaguedWarrior.size() - 1);
                        target->CastSpell(nullptr, spellSummonPlaguedWarrior[index], TRIGGERED_OLD_TRIGGERED);
                        spellSummonPlaguedWarrior.erase(spellSummonPlaguedWarrior.begin() + index);
                    }
                }
                else
                {
                    target->CastSpell(nullptr, SPELL_SUMMON_WARRIOR_THREE, TRIGGERED_OLD_TRIGGERED);
                }
            }
            case 27: // 135s
            case 33: // 165s
            case 63: // 315s
            case 69: // 345s
            case 99: // 495s
            case 105: // 525s
            {
                DoBroadcastText(EMOTE_SKELETON, target);

                std::vector<uint32> spellSummonPlaguedChampion =
                {
                    SPELL_SUMMON_CHAMP01, SPELL_SUMMON_CHAMP02, SPELL_SUMMON_CHAMP03, SPELL_SUMMON_CHAMP04, SPELL_SUMMON_CHAMP05, SPELL_SUMMON_CHAMP06, SPELL_SUMMON_CHAMP07, SPELL_SUMMON_CHAMP08, SPELL_SUMMON_CHAMP09, SPELL_SUMMON_CHAMP10
                };

                std::vector<uint32> spellSummonPlaguedGuardian =
                {
                    SPELL_SUMMON_GUARD01, SPELL_SUMMON_GUARD02, SPELL_SUMMON_GUARD03, SPELL_SUMMON_GUARD04
                };

                // A bit unclear how many in each sub phase
                switch (aura->GetAuraTicks())
                {
                    case 27: // 135s
                    case 33: // 165s
                    {
                        for (uint8 i = 0; i < (regularMode ? 2 : 4); ++i)
                        {
                            uint32 index = urand(0, spellSummonPlaguedChampion.size() - 1);
                            target->CastSpell(nullptr, spellSummonPlaguedChampion[index], TRIGGERED_OLD_TRIGGERED);
                            spellSummonPlaguedChampion.erase(spellSummonPlaguedChampion.begin() + index);
                        }

                        break;
                    }
                    case 63: // 315s
                    case 69: // 345s
                    {
                        for (uint8 i = 0; i < (regularMode ? 1 : 2); ++i)
                        {
                            uint32 index = urand(0, spellSummonPlaguedChampion.size() - 1);
                            target->CastSpell(nullptr, spellSummonPlaguedChampion[index], TRIGGERED_OLD_TRIGGERED);
                            spellSummonPlaguedChampion.erase(spellSummonPlaguedChampion.begin() + index);
                            index = urand(0, spellSummonPlaguedGuardian.size() - 1);
                            target->CastSpell(nullptr, spellSummonPlaguedGuardian[index], TRIGGERED_OLD_TRIGGERED);
                            spellSummonPlaguedGuardian.erase(spellSummonPlaguedGuardian.begin() + index);
                        }
                        break;
                    }
                    case 99: // 495s
                    case 105: // 525s
                    {
                        for (uint8 i = 0; i < (regularMode ? 2 : 4); ++i)
                        {
                            uint32 index = urand(0, spellSummonPlaguedGuardian.size() - 1);
                            target->CastSpell(nullptr, spellSummonPlaguedGuardian[index], TRIGGERED_OLD_TRIGGERED);
                            spellSummonPlaguedGuardian.erase(spellSummonPlaguedGuardian.begin() + index);
                        }

                        break;
                    }
                }
                break;
            }
            case 108: // 540s berserk
                target->CastSpell(nullptr, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }
};

void AddSC_boss_noth()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_noth";
    pNewScript->GetAI = &GetNewAIInstance<boss_nothAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonSkeletonsNoth>("spell_summon_skeletons_noth");
}
