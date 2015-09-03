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
SDName: Boss_Supremus
SD%Complete: 90
SDComment: Unknown if other speed-changes happen, remove AI for trigger mobs in next step
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    EMOTE_NEW_TARGET                = -1564010,
    EMOTE_PUNCH_GROUND              = -1564011,
    EMOTE_GROUND_CRACK              = -1564012,

    // Spells
    SPELL_HATEFUL_STRIKE            = 41926,
    SPELL_CHARGE                    = 41581,
    SPELL_MOLTEN_FLAME              = 40980,
    SPELL_VOLCANIC_ERUPTION_BOSS    = 40276,
    SPELL_VOLCANIC_ERUPTION_VOLCANO = 40117,
    SPELL_MOLTEN_PUNCH              = 40126,
    SPELL_BERSERK                   = 45078,
    SPELL_SLOW_SELF                 = 41922,

    NPC_VOLCANO                     = 23085,
    NPC_STALKER                     = 23095,
};

/* Non existed spells that were used in 3.2
 * Stalker:  40257 41930
 * Supremus: 33420 41582 41925 41951
 */

const float RANGE_MOLTEN_PUNCH      = 40.0;

/* These floats are related to the speed-hack near end of script;
 * Statet at wowwiki: "If the gaze target is further away than 40 yards, he dashes at about five times the normal run speed until the range is about 20 yards."
 * TODO But this is currently not confirmed otherwise to be actually happening
 * const float RANGE_MIN_DASHING       = 20.0;
 * const float SPEED_DASHING           = 5.0;
 * const float SPEED_CHASE             = 0.9f;
 */

// TODO Remove this 'script' when combat movement can be proper prevented from core-side
struct molten_flameAI : public Scripted_NoMovementAI
{
    molten_flameAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override {}
    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

// TODO Remove this 'script' when combat movement can be proper prevented from core-side
struct npc_volcanoAI : public Scripted_NoMovementAI
{
    npc_volcanoAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override {}
    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

struct boss_supremusAI : public ScriptedAI
{
    boss_supremusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSummonFlameTimer;
    uint32 m_uiSwitchTargetTimer;
    uint32 m_uiPhaseSwitchTimer;
    uint32 m_uiSummonVolcanoTimer;
    uint32 m_uiHatefulStrikeTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiMoltenPunchTimer;

    bool m_bTankPhase;

    GuidList m_lSummonedGUIDs;

    void Reset() override
    {
        m_uiHatefulStrikeTimer = 5000;
        m_uiSummonFlameTimer   = 20000;
        m_uiPhaseSwitchTimer   = 60000;
        m_uiMoltenPunchTimer   = 8000;
        m_uiBerserkTimer       = 15 * MINUTE * IN_MILLISECONDS;

        m_bTankPhase = true;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SUPREMUS, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SUPREMUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SUPREMUS, DONE);

        for (GuidList::const_iterator itr = m_lSummonedGUIDs.begin(); itr != m_lSummonedGUIDs.end(); ++itr)
        {
            if (Creature* pSummoned = m_creature->GetMap()->GetCreature(*itr))
                pSummoned->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_STALKER)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (pTarget)
            {
                pSummoned->GetMotionMaster()->Clear();
                pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0.0f);
                pSummoned->CastSpell(pSummoned, SPELL_MOLTEN_FLAME, false, NULL, NULL, m_creature->GetObjectGuid());
            }
        }

        else if (pSummoned->GetEntry() == NPC_VOLCANO)
            pSummoned->CastSpell(pSummoned, SPELL_VOLCANIC_ERUPTION_VOLCANO, false, NULL, NULL, m_creature->GetObjectGuid());
    }

    Unit* GetHatefulStrikeTarget()
    {
        uint32 uiHealth = 0;
        Unit* pTarget = NULL;

        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator iter = tList.begin(); iter != tList.end(); ++iter)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid());

            if (pUnit && m_creature->CanReachWithMeleeAttack(pUnit))
            {
                if (pUnit->GetHealth() > uiHealth)
                {
                    uiHealth = pUnit->GetHealth();
                    pTarget = pUnit;
                }
            }
        }

        return pTarget;
    }

    void KilledUnit(Unit* pKilled) override
    {
        // The current target is the fixated target - repick a new one
        if (!m_bTankPhase && pKilled == m_creature->getVictim())
            m_uiSwitchTargetTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiSummonFlameTimer < uiDiff)
        {
            // This currently is entirely screwed, because the npc is summoned somewhere far away as of big bounding box of supremus
            if (DoCastSpellIfCan(m_creature, SPELL_MOLTEN_PUNCH) == CAST_OK)
                m_uiSummonFlameTimer = 20000;
        }
        else
            m_uiSummonFlameTimer -= uiDiff;

        if (m_uiPhaseSwitchTimer < uiDiff)
        {
            if (!m_bTankPhase)
            {
                m_bTankPhase = true;
                m_creature->RemoveAurasDueToSpell(SPELL_SLOW_SELF);
                m_creature->FixateTarget(NULL);
            }
            else
            {
                m_bTankPhase = false;
                m_uiSwitchTargetTimer = 0;
                m_uiSummonVolcanoTimer = 2000;

                DoCastSpellIfCan(m_creature, SPELL_SLOW_SELF, CAST_INTERRUPT_PREVIOUS);
            }

            m_uiPhaseSwitchTimer = MINUTE * IN_MILLISECONDS;
        }
        else
            m_uiPhaseSwitchTimer -= uiDiff;

        if (m_bTankPhase)
        {
            if (m_uiHatefulStrikeTimer < uiDiff)
            {
                if (Unit* pTarget = GetHatefulStrikeTarget())
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_HATEFUL_STRIKE) == CAST_OK)
                        m_uiHatefulStrikeTimer = 5000;
                }
            }
            else
                m_uiHatefulStrikeTimer -= uiDiff;
        }
        else                                                // !m_bTankPhase
        {
            if (m_uiSwitchTargetTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->FixateTarget(pTarget);
                    DoScriptText(EMOTE_NEW_TARGET, m_creature);
                    m_uiSwitchTargetTimer = 10000;
                }
            }
            else
                m_uiSwitchTargetTimer -= uiDiff;

            if (m_uiSummonVolcanoTimer < uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);

                if (DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_VOLCANIC_ERUPTION_BOSS) == CAST_OK)
                {
                    DoScriptText(EMOTE_GROUND_CRACK, m_creature);
                    m_uiSummonVolcanoTimer = 10000;
                }
            }
            else
                m_uiSummonVolcanoTimer -= uiDiff;

            if (m_uiMoltenPunchTimer < uiDiff)
            {
                if (m_creature->GetCombatDistance(m_creature->getVictim(), false) < RANGE_MOLTEN_PUNCH)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHARGE);
                    DoScriptText(EMOTE_PUNCH_GROUND, m_creature);
                }
                m_uiMoltenPunchTimer = 8000;                // might be better with small timer and some sort of cast-chance
            }
            else
                m_uiMoltenPunchTimer -= uiDiff;

            /* Not understood how this really must work
             * if (m_creature->GetSpeedRate(MOVE_RUN) > SPEED_CHASE && m_creature->GetCombatDistance(m_creature->getVictim()) < RANGE_MIN_DASHING)
             *     m_creature->SetSpeedRate(MOVE_RUN, SPEED_CHASE);
             * else if (m_creature->GetCombatDistance(m_creature->getVictim()) > RANGE_MOLTEN_PUNCH)
             *     m_creature->SetSpeedRate(MOVE_RUN, SPEED_DASHING);
             */
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_supremus(Creature* pCreature)
{
    return new boss_supremusAI(pCreature);
}

CreatureAI* GetAI_molten_flame(Creature* pCreature)
{
    return new molten_flameAI(pCreature);
}

CreatureAI* GetAI_npc_volcano(Creature* pCreature)
{
    return new npc_volcanoAI(pCreature);
}

void AddSC_boss_supremus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_supremus";
    pNewScript->GetAI = &GetAI_boss_supremus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "molten_flame";
    pNewScript->GetAI = &GetAI_molten_flame;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_volcano";
    pNewScript->GetAI = &GetAI_npc_volcano;
    pNewScript->RegisterSelf();
}
