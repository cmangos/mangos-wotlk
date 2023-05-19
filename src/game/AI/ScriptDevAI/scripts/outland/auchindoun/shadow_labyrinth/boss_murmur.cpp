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
SDName: Boss_Murmur
SD%Complete: 75
SDComment: Sonic Boom and Murmur's Touch require additional research and core support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shadow_labyrinth.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    EMOTE_SONIC_BOOM            = -1555036,

    // Intro spells - used on npcs
    SPELL_SUPPRESSION_BLAST     = 33332,
    SPELL_MURMURS_WRATH         = 33331,
    SPELL_MURMURS_WRATH_2       = 33329,

    SPELL_MAGNETIC_PULL         = 33689,
    SPELL_SONIC_BOOM            = 33923,        // dummy spell - triggers 33666
    SPELL_SONIC_BOOM_H          = 38796,        // dummy spell - triggers 38795
    SPELL_MURMURS_TOUCH         = 33711,        // on expire silences the party members using shockwave - 33686 - also related to spell 33760
    SPELL_MURMURS_TOUCH_H       = 38794,
    SPELL_RESONANCE             = 33657,

    SPELL_SONIC_SHOCK           = 38797,        // Heroic Spell
    SPELL_THUNDERING_STORM      = 39365,        // Heroic Spell
};

/* Murmurs Spell Timers - NORMAL/HEROIC */

//Sonic Boom
static const uint32 SPELL_SONIC_BOOM_MIN_TIMER = 31000;               //31 seconds (Both)
static const uint32 SPELL_SONIC_BOOM_MAX_TIMER = 38000;               //38 seconds (Both)

//Murmurs Touch
static const uint32 SPELL_MURMURS_TOUCH_TIMER_N = 21000;              //21 seconds (Normal)
static const uint32 SPELL_MURMURS_TOUCH_MIN_TIMER_H = 29000;          //29 seconds (Heroic)
static const uint32 SPELL_MURMURS_TOUCH_MAX_TIMER_H = 40000;          //40 seconds (Heroic)

//Resonance
static const uint32 SPELL_RESONANCE_MIN_TIMER = 5000;                 //5 seconds (Both)
static const uint32 SPELL_RESONANCE_MAX_TIMER = 12000;                //12 seconds (Both)

//Magnetic Pull
static const uint32 SPELL_MAGNETIC_PULL_MIN_TIMER = 21000;            //21 seconds (Both)
static const uint32 SPELL_MAGNETIC_PULL_MAX_TIMER = 30000;            //30 seconds (Both)

//Sonic Shock
static const uint32 SPELL_SONIC_SHOCK_MIN_TIMER = 3000;              //3 seconds (Both)
static const uint32 SPELL_SONIC_SHOCK_MAX_TIMER = 10000;             //10 seconds (Both)

//Thundering Storm
static const float SPELL_THUNDERING_STORM_MINRANGE = 34;             // 34 yards from boss
static const float SPELL_THUNDERING_STORM_MAXRANGE = 100;            // 100 yards from boss
static const uint32 SPELL_THUNDERING_STORM_MIN_TIMER = 6000;         //15 seconds (Both)
static const uint32 SPELL_THUNDERING_STORM_MAX_TIMER = 10000;        //15 seconds (Both)

struct boss_murmurAI : public Scripted_NoMovementAI
{
    boss_murmurAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_thunderingParams.range.minRange = SPELL_THUNDERING_STORM_MINRANGE;
        m_thunderingParams.range.maxRange = SPELL_THUNDERING_STORM_MAXRANGE;
        m_uiCastersAttackMurmurTimer = 0;
        m_uiAttackTimer = 0;
        SetAIImmobilizedState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiAttackTimer;
    uint32 m_uiCastersAttackMurmurTimer;

    GuidVector spellbindersVector;
    GuidVector summonersVector;

    uint32 m_uiSonicBoomTimer;
    uint32 m_uiMurmursTouchTimer;
    uint32 m_uiResonanceTimer;
    uint32 m_uiMagneticPullTimer;
    uint32 m_uiSonicShockTimer;
    uint32 m_uiThunderingStormTimer;

    SelectAttackingTargetParams m_thunderingParams;

    void Reset() override
    {
        m_uiSonicBoomTimer       = urand(SPELL_SONIC_BOOM_MIN_TIMER, SPELL_SONIC_BOOM_MAX_TIMER);
        m_uiMurmursTouchTimer    = m_bIsRegularMode ? SPELL_MURMURS_TOUCH_TIMER_N : urand(SPELL_MURMURS_TOUCH_MIN_TIMER_H, SPELL_MURMURS_TOUCH_MAX_TIMER_H);
        m_uiResonanceTimer       = urand(SPELL_RESONANCE_MIN_TIMER, SPELL_RESONANCE_MAX_TIMER);
        m_uiMagneticPullTimer    = urand(SPELL_MAGNETIC_PULL_MIN_TIMER, SPELL_MAGNETIC_PULL_MAX_TIMER);
        m_uiSonicShockTimer      = urand(SPELL_SONIC_SHOCK_MIN_TIMER, SPELL_SONIC_SHOCK_MAX_TIMER);
        m_uiThunderingStormTimer = urand(SPELL_THUNDERING_STORM_MIN_TIMER, SPELL_THUNDERING_STORM_MAX_TIMER);

        // Boss has only 0.4 of max health
        m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.4));
    }


    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_uiCastersAttackMurmurTimer = urand(8000, 10000);
            m_uiAttackTimer = urand(8000, 10000);
            m_pInstance->GetCreatureGuidVectorFromStorage(NPC_CABAL_SPELLBINDER, spellbindersVector);
            m_pInstance->GetCreatureGuidVectorFromStorage(NPC_CABAL_SUMMONER, summonersVector);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->IsInCombat())
        {
            if (m_uiAttackTimer)
            {
                if (m_uiAttackTimer <= uiDiff)
                {
                    // kill one that's moving
                    if (urand(0, 1))
                    {
                        GuidVector moversVector;
                        for (ObjectGuid& guid : spellbindersVector)
                        {
                            if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                            {
                                if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                                {
                                    moversVector.push_back(guid);
                                }
                            }
                        }
                        for (ObjectGuid& guid : summonersVector)
                        {
                            if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                            {
                                if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                                {
                                    moversVector.push_back(guid);
                                }
                            }
                        }
                        if (moversVector.size() > 0)
                        {
                            if (ObjectGuid& guid = moversVector[urand(0, moversVector.size() - 1)])
                            {
                                if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                                {
                                    DoCastSpellIfCan(creature, SPELL_MURMURS_WRATH);
                                }
                            }
                        }
                    }
                    // stun 5 targets
                    else
                        DoCastSpellIfCan(m_creature, SPELL_SUPPRESSION_BLAST);

                    m_uiAttackTimer = 3000;
                }
                else
                    m_uiAttackTimer -= uiDiff;
            }

            if (m_uiCastersAttackMurmurTimer)
            {
                if (m_uiCastersAttackMurmurTimer <= uiDiff)
                {
                    for (ObjectGuid& guid : spellbindersVector)
                    {
                        if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                        {
                            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, creature);
                        }
                    }
                    for (ObjectGuid& guid : summonersVector)
                    {
                        if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                        {
                            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, creature);
                        }
                    }
                    m_uiCastersAttackMurmurTimer = urand(3000, 8000);
                }
                else
                    m_uiCastersAttackMurmurTimer -= uiDiff;
            }
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget())
            return;

        // SonicBoom_Timer
        if (m_uiSonicBoomTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SONIC_BOOM : SPELL_SONIC_BOOM_H) == CAST_OK)
            {
                DoScriptText(EMOTE_SONIC_BOOM, m_creature);
                m_uiSonicBoomTimer = urand(SPELL_SONIC_BOOM_MIN_TIMER, SPELL_SONIC_BOOM_MAX_TIMER);
            }
        }
        else
            m_uiSonicBoomTimer -= uiDiff;

        // MurmursTouch_Timer
        if (m_uiMurmursTouchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MURMURS_TOUCH : SPELL_MURMURS_TOUCH_H) == CAST_OK)
                m_uiMurmursTouchTimer = m_bIsRegularMode ? SPELL_MURMURS_TOUCH_TIMER_N : urand(SPELL_MURMURS_TOUCH_MIN_TIMER_H, SPELL_MURMURS_TOUCH_MAX_TIMER_H);
        }
        else
            m_uiMurmursTouchTimer -= uiDiff;

        if (m_bIsRegularMode)
        {
            // Magnetic Pull
            if (m_uiMagneticPullTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MAGNETIC_PULL, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_MAGNETIC_PULL) == CAST_OK)
                        m_uiMagneticPullTimer = urand(SPELL_MAGNETIC_PULL_MIN_TIMER, SPELL_MAGNETIC_PULL_MAX_TIMER);
                }
            }
            else
                m_uiMagneticPullTimer -= uiDiff;
        }
        else
        {
		    //Sonic Shock
            if (m_uiSonicShockTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SONIC_SHOCK, SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SONIC_SHOCK) == CAST_OK)
                        m_uiSonicShockTimer = urand(SPELL_SONIC_SHOCK_MIN_TIMER, SPELL_SONIC_SHOCK_MAX_TIMER);
                }
            }
            else
                m_uiSonicShockTimer -= uiDiff;

            // Thundering Storm
            if (m_uiThunderingStormTimer < uiDiff)
            {
                m_uiThunderingStormTimer = 0;
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_FARTHEST_AWAY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_thunderingParams))
                {
                    if (DoCastSpellIfCan(target, SPELL_THUNDERING_STORM) == CAST_OK)
                        m_uiThunderingStormTimer = urand(SPELL_THUNDERING_STORM_MIN_TIMER, SPELL_THUNDERING_STORM_MAX_TIMER);
                }                
            }
            else
                m_uiThunderingStormTimer -= uiDiff;
        }

        // Resonance_Timer - cast if no target is in range
        if (m_creature->GetVictim() && m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
            DoMeleeAttackIfReady();
        else
        {
            if (m_uiResonanceTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_RESONANCE) == CAST_OK)
                    m_uiResonanceTimer = urand(SPELL_RESONANCE_MIN_TIMER, SPELL_RESONANCE_MAX_TIMER);
            }
            else
                m_uiResonanceTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_murmur(Creature* pCreature)
{
    return new boss_murmurAI(pCreature);
}

struct SuppressionBlast : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(5);
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsInCombat())
            return false;

        return true;
    }
};

void AddSC_boss_murmur()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_murmur";
    pNewScript->GetAI = &GetAI_boss_murmur;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SuppressionBlast>("spell_suppression_blast");
}
