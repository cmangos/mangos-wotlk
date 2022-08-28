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

    PHASE_GROUND                        = 0,
    PHASE_BALCONY                       = 1,

    PHASE_SKELETON_1                    = 1,
    PHASE_SKELETON_2                    = 2,
    PHASE_SKELETON_3                    = 3
};

enum NothAction
{
    NOTH_BLINK,
    NOTH_CURSE,
    NOTH_SUMMON,
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
        AddCombatAction(NOTH_BLINK, true);
        AddCombatAction(NOTH_CURSE, 4s);
        AddCombatAction(NOTH_SUMMON, 12s);
        AddCombatAction(NOTH_PHASE_BALCONY, 90s);
        AddCombatAction(NOTH_PHASE_GROUND, true);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;
    GuidList m_lUndeadSummonGuidList;

    uint8 m_uiPhase, m_uiPhaseSub;

    void Reset() override
    {
        m_uiPhase = PHASE_GROUND;
        m_uiPhaseSub = PHASE_GROUND;

        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
        SetReactState(REACT_AGGRESSIVE);
        SetRootSelf(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (!m_isRegularMode)
            ResetCombatAction(NOTH_BLINK, 25s);
        BossAI::Aggro();
    }

    void EnterEvadeMode() override
    {
        if (m_instance->GetPlayerInMap(true, false))
            return;
        BossAI::EnterEvadeMode();
        m_creature->ForcedDespawn();
        m_creature->SetRespawnDelay(10 * IN_MILLISECONDS, true);
        m_creature->Respawn();

        for (GuidList::const_iterator itr = m_lUndeadSummonGuidList.begin(); itr != m_lUndeadSummonGuidList.end(); ++itr)
        {
            if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
            {
                summon->ForcedDespawn(2000);
            }
        }
        JustReachedHome();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (!summoned->HasAura(384152)) //Custom spell for 30% increased difficulty. *NOT* accurate to 3.3.5a
            summoned->CastSpell(summoned, 384152, TRIGGERED_OLD_TRIGGERED);
        summoned->AI()->AddCustomAction(SUMMONED_UNROOT, 3s + 500ms, [summoned]()
        {
            if (summoned && summoned->AI())
                summoned->AI()->SetRootSelf(false);
        });
        summoned->AI()->SetRootSelf(true);
        summoned->SetInCombatWithZone();
        m_lUndeadSummonGuidList.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        m_lUndeadSummonGuidList.remove(summoned->GetObjectGuid());
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pCaster == m_creature && pSpell->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEAP)
            DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_CRIPPLE : SPELL_CRIPPLE_H);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case NOTH_BLINK: return 25s;
            case NOTH_CURSE: return 28s;
            case NOTH_SUMMON: return 30s;
            case NOTH_PHASE_GROUND:
            {
                switch (m_uiPhaseSub)               // Set Duration of Skeleton phase
                {
                    case PHASE_SKELETON_1: return 70s;
                    case PHASE_SKELETON_2: return 97s;
                    case PHASE_SKELETON_3: return 120s;
                }
            }
            case NOTH_PHASE_BALCONY:
            {
                switch (m_uiPhaseSub)
                {
                    case PHASE_SKELETON_1: return 110s;
                    case PHASE_SKELETON_2: return 180s;
                    case PHASE_SKELETON_3:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
                        return -1ms;
                    }
                }
            }
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NOTH_BLINK:
            {
                static uint32 const auiSpellBlink[4] =
                {
                    SPELL_BLINK_1, SPELL_BLINK_2, SPELL_BLINK_3, SPELL_BLINK_4
                };

                if (DoCastSpellIfCan(m_creature, auiSpellBlink[urand(0, 3)]) == CAST_OK)
                {
                    DoResetThreat();
                    break;
                }
                return;
            }
            case NOTH_CURSE:
            {
                DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_CURSE_PLAGUEBRINGER : SPELL_CURSE_PLAGUEBRINGER_H);
                break;
            }
            case NOTH_SUMMON:
            {
                if (m_uiPhase == PHASE_GROUND)
                {
                    DoBroadcastText(SAY_SUMMON, m_creature);
                    DoBroadcastText(EMOTE_WARRIOR, m_creature);

                    if (m_isRegularMode)
                    {
                        static uint32 const auiSpellSummonPlaguedWarrior[3] =
                        {
                            SPELL_SUMMON_WARRIOR_1, SPELL_SUMMON_WARRIOR_2, SPELL_SUMMON_WARRIOR_3
                        };

                        for (uint8 i = 0; i < 2; ++i)
                            DoCastSpellIfCan(m_creature, auiSpellSummonPlaguedWarrior[urand(0, 2)], CAST_TRIGGERED);
                    }
                    else
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WARRIOR_THREE, CAST_TRIGGERED);
                    }
                    break;
                }
                else
                {
                    DoBroadcastText(EMOTE_SKELETON, m_creature);

                    static uint32 const auiSpellSummonPlaguedChampion[10] =
                    {
                        SPELL_SUMMON_CHAMP01, SPELL_SUMMON_CHAMP02, SPELL_SUMMON_CHAMP03, SPELL_SUMMON_CHAMP04, SPELL_SUMMON_CHAMP05, SPELL_SUMMON_CHAMP06, SPELL_SUMMON_CHAMP07, SPELL_SUMMON_CHAMP08, SPELL_SUMMON_CHAMP09, SPELL_SUMMON_CHAMP10
                    };

                    static uint32 const auiSpellSummonPlaguedGuardian[4] =
                    {
                        SPELL_SUMMON_GUARD01, SPELL_SUMMON_GUARD02, SPELL_SUMMON_GUARD03, SPELL_SUMMON_GUARD04
                    };

                    // A bit unclear how many in each sub phase
                    switch (m_uiPhaseSub)
                    {
                        case PHASE_SKELETON_1:
                        {
                            for (uint8 i = 0; i < (m_isRegularMode ? 2 : 4); ++i)
                                DoCastSpellIfCan(m_creature, auiSpellSummonPlaguedChampion[urand(0, 9)], CAST_TRIGGERED);

                            break;
                        }
                        case PHASE_SKELETON_2:
                        {
                            for (uint8 i = 0; i < (m_isRegularMode ? 1 : 2); ++i)
                            {
                                DoCastSpellIfCan(m_creature, auiSpellSummonPlaguedChampion[urand(0, 9)], CAST_TRIGGERED);
                                DoCastSpellIfCan(m_creature, auiSpellSummonPlaguedGuardian[urand(0, 3)], CAST_TRIGGERED);
                            }
                            break;
                        }
                        case PHASE_SKELETON_3:
                        {
                            for (uint8 i = 0; i < (m_isRegularMode ? 2 : 4); ++i)
                                DoCastSpellIfCan(m_creature, auiSpellSummonPlaguedGuardian[urand(0, 3)], CAST_TRIGGERED);

                            break;
                        }
                    }
                    break;
                }
            }
            case NOTH_PHASE_BALCONY:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                {
                    static const std::vector<uint32> GROUND_PHASE_ACTIONS = {NOTH_BLINK, NOTH_CURSE, NOTH_PHASE_BALCONY};
                    for (uint32 action : GROUND_PHASE_ACTIONS)
                        DisableCombatAction(action);

                    DoBroadcastText(EMOTE_TELEPORT, m_creature);
                    SetRootSelf(true);
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
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_RETURN) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_TELEPORT_RETURN, m_creature);
                    SetRootSelf(false);
                    m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    SetMeleeEnabled(true);
                    HandleTargetRestoration();
                    m_uiPhase = PHASE_GROUND;
                    ResetCombatAction(NOTH_PHASE_BALCONY, GetSubsequentActionTimer(NOTH_PHASE_BALCONY));
                    ResetCombatAction(NOTH_CURSE, GetSubsequentActionTimer(NOTH_CURSE));
                    if (!m_isRegularMode)
                        ResetCombatAction(NOTH_BLINK, GetSubsequentActionTimer(NOTH_BLINK));
                }
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

void AddSC_boss_noth()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_noth";
    pNewScript->GetAI = &GetNewAIInstance<boss_nothAI>;
    pNewScript->RegisterSelf();
}
