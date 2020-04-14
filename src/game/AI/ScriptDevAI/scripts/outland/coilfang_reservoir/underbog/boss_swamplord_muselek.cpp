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
SDName: boss_swamplord_muselek
SD%Complete: 95
SDComment: Implementing in SD2 due to some complex behavior
SDCategory: Coilfang Resevoir, Underbog
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "underbog.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_AGGRO_1 = -1546000,
    SAY_AGGRO_2 = -1546001,
    SAY_AGGRO_3 = -1546002,
    SAY_COMMAND = -1546003,
    SAY_SLAY_1  = -1546004,
    SAY_SLAY_2  = -1546005,
    SAY_DEATH   = -1546006,
    
    // SWAMPLORD
    SPELL_KNOCK_AWAY           = 18813,
    SPELL_SHOOT                = 22907,
    SPELL_NOTIFY_OF_DEATH      = 31547, // TODO: what does this do?
    SPELL_RAPTOR_STRIKE        = 31566,
    SPELL_DETERRENCE           = 31567,
    SPELL_HUNTERS_MARK         = 31615,
    SPELL_AIMED_SHOT           = 31623,
    SPELL_FREEZING_TRAP_EFFECT = 31932,
    SPELL_THROW_FREEZING_TRAP  = 31946,
    SPELL_BEAR_COMMAND         = 34662,
    SPELL_MULTI_SHOT           = 34974,

    // CLAW
    SPELL_ROAR   = 31429,
    SPELL_MAUL   = 34298,
    SPELL_FRENZY = 34971,
};

enum MuselekActions
{
    MUSELEK_ACTION_KNOCK_AWAY,
    MUSELEK_ACTION_RAPTOR_STRIKE,
    MUSELEK_ACTION_BEAR_COMMAND,
    MUSELEK_ACTION_RANGED_ATTACK,
    MUSELEK_ACTION_DETERRENCE,
    MUSELEK_ACTION_MAX,
};

struct boss_swamplord_muselekAI : public ScriptedAI, public CombatActions
{
    boss_swamplord_muselekAI(Creature* creature) : ScriptedAI(creature), CombatActions(MUSELEK_ACTION_MAX)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        AddCombatAction(MUSELEK_ACTION_KNOCK_AWAY, 0u);
        AddCombatAction(MUSELEK_ACTION_RAPTOR_STRIKE, 0u);
        AddCombatAction(MUSELEK_ACTION_BEAR_COMMAND, 0u);
        AddCombatAction(MUSELEK_ACTION_RANGED_ATTACK, 0u);
        AddCombatAction(MUSELEK_ACTION_DETERRENCE, 0u);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_isRegularMode;
    bool m_isInAimShotSequence;
    bool m_firstTrapThrown;
    bool m_secondTrapThrown;

    float m_fMinRange = 8.f;
    float m_fMaxRange = 30.f;

    ObjectGuid m_AimedShotTarget;

    void Reset() override
    {
        for (uint32 i = 0; i < MUSELEK_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(MUSELEK_ACTION_KNOCK_AWAY, GetInitialActionTimer(MUSELEK_ACTION_KNOCK_AWAY));
        ResetTimer(MUSELEK_ACTION_RAPTOR_STRIKE, GetInitialActionTimer(MUSELEK_ACTION_RAPTOR_STRIKE));
        ResetTimer(MUSELEK_ACTION_BEAR_COMMAND, GetInitialActionTimer(MUSELEK_ACTION_BEAR_COMMAND));
        ResetTimer(MUSELEK_ACTION_RANGED_ATTACK, GetInitialActionTimer(MUSELEK_ACTION_RANGED_ATTACK));
        ResetTimer(MUSELEK_ACTION_DETERRENCE, GetInitialActionTimer(MUSELEK_ACTION_DETERRENCE));

        DisableCombatAction(MUSELEK_ACTION_DETERRENCE);

        m_isInAimShotSequence = false;
        m_firstTrapThrown  = false;
        m_secondTrapThrown = false;

        m_AimedShotTarget.Clear();

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case MUSELEK_ACTION_KNOCK_AWAY: return urand(25000, 30000);
            case MUSELEK_ACTION_RAPTOR_STRIKE: return urand(1500, 4000);
            case MUSELEK_ACTION_BEAR_COMMAND: return urand(8000, 12000);
            case MUSELEK_ACTION_RANGED_ATTACK: return urand(500, 1250);
            case MUSELEK_ACTION_DETERRENCE: return urand(15000, 20000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case MUSELEK_ACTION_KNOCK_AWAY: return urand(16000, 32000);
            case MUSELEK_ACTION_RAPTOR_STRIKE: return urand(6000, 9000);
            case MUSELEK_ACTION_BEAR_COMMAND: return 25000;
            case MUSELEK_ACTION_RANGED_ATTACK: return urand(2000, 4000);
            case MUSELEK_ACTION_DETERRENCE: return urand(18000, 26000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < MUSELEK_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case MUSELEK_ACTION_KNOCK_AWAY:
                    {
                        //DoCastSpellIfCan(m_creature->GetVictim(), SPELL_KNOCK_AWAY, TRIGGERED_NONE);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MUSELEK_ACTION_RAPTOR_STRIKE:
                    {
                        DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAPTOR_STRIKE, TRIGGERED_NONE);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MUSELEK_ACTION_BEAR_COMMAND:
                    {
                        if (Creature *claw = m_pInstance->GetSingleCreatureFromStorage(NPC_CLAW))
                            if (claw->IsAlive() && claw->GetEntry() != NPC_CLAW_DRUID_FORM)
                            {
                                uint8 claw_spell = urand(0, 2);

                                switch (claw_spell)
                                {
                                    case 0:
                                        claw->AI()->DoCastSpellIfCan(claw, SPELL_MAUL, TRIGGERED_NONE);
                                        break;
                                    case 1:
                                        claw->AI()->DoCastSpellIfCan(claw, SPELL_ROAR, TRIGGERED_NONE);
                                        break;
                                    default:
                                        break;
                                }

                                claw->AI()->DoCastSpellIfCan(claw, SPELL_FRENZY, TRIGGERED_NONE);
                                DoScriptText(SAY_COMMAND, m_creature);
                            }

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MUSELEK_ACTION_RANGED_ATTACK:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, SPELL_SHOOT, SELECT_FLAG_IN_LOS))
                        {
                            bool shoot = false;

                            if (m_creature->hasUnitState(UNIT_STAT_NO_COMBAT_MOVEMENT)) // Has No Movement Flag
                            {
                                if (m_creature->GetDistance(pTarget) < m_fMinRange
                                    || m_creature->GetDistance(pTarget) > m_fMaxRange)
                                    SetCombatMovement(true);
                                else
                                    shoot = true;
                            }
                            else if (m_creature->GetDistance(pTarget) >= m_fMinRange) // Is Chasing
                                shoot = true;

                            if (shoot)
                            {
                                uint32 RangedSpell = urand(0, 2) ? SPELL_SHOOT : SPELL_MULTI_SHOT; // 66% shoot, 33% multishot

                                SetCombatMovement(false);
                                DoCastSpellIfCan(pTarget, RangedSpell, TRIGGERED_NONE);
                            }
                        }
                        else if(m_creature->hasUnitState(UNIT_STAT_NO_COMBAT_MOVEMENT)) // Has No Movement Flag but no valid target
                            SetCombatMovement(true);

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case MUSELEK_ACTION_DETERRENCE:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_DETERRENCE, TRIGGERED_NONE);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                }
            }
        }
    }
    
    void DoBeginAimedShotSequence()
    {
        m_isInAimShotSequence = true;
        DistanceYourself();
    }

    void DistanceYourself()
    {
        if (!m_isInAimShotSequence)
            return;

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_AIMED_SHOT, SELECT_FLAG_PLAYER))
        {
            m_AimedShotTarget = pTarget->GetObjectGuid();
            DoCastSpellIfCan(pTarget, SPELL_HUNTERS_MARK); // this casts on everyone?
            float distance = DISTANCING_CONSTANT + m_creature->GetCombinedCombatReach(m_creature->GetVictim(), true) * 3;
            m_creature->GetMotionMaster()->DistanceYourself(distance);
        }
    }

    void DistancingStarted()
    {
        SetCombatScriptStatus(true);
    }
    
    void DistancingEnded()
    {
        m_isInAimShotSequence = false;
        SetCombatScriptStatus(false);

        ResetTimer(MUSELEK_ACTION_RANGED_ATTACK, GetSubsequentActionTimer(MUSELEK_ACTION_RANGED_ATTACK));
        SetActionReadyStatus(MUSELEK_ACTION_RANGED_ATTACK, false);

        if (Unit* pTarget = m_creature->GetMap()->GetPlayer(m_AimedShotTarget))
            if (pTarget->IsAlive())
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_AIMED_SHOT, TRIGGERED_NONE);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell) override 
    {
        if (spell->Id == SPELL_FREEZING_TRAP_EFFECT)
        {
            if (!m_isInAimShotSequence)
                DoBeginAimedShotSequence();
            if (Creature *claw = m_pInstance->GetSingleCreatureFromStorage(NPC_CLAW))
                if (claw->IsAlive() && claw->GetEntry() != NPC_CLAW_DRUID_FORM)
                    claw->getThreatManager().modifyThreatPercent(target, (0 - urand(30, 99))); // Freezing trap messes with bear aggro
        }
    }

    void Aggro(Unit* /*who*/)
    {
        uint8 yell = urand(0, 2);
        switch (yell)
        {
            case 0:
                DoScriptText(SAY_AGGRO_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_AGGRO_2, m_creature);
                break;
            case 2:
                DoScriptText(SAY_AGGRO_3, m_creature);
                break;
        }	
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature *claw = m_pInstance->GetSingleCreatureFromStorage(NPC_CLAW))
            m_creature->CastSpell(claw, SPELL_NOTIFY_OF_DEATH, TRIGGERED_NONE); // TODO: what does this do?
        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override 
    {
        if (Creature *claw = m_pInstance->GetSingleCreatureFromStorage(NPC_CLAW))
            if (claw->GetEntry() == NPC_CLAW_DRUID_FORM)
            {
                claw->ForcedDespawn();
                claw->Respawn();
            }
    }

    void KilledUnit(Unit* /*victim*/) override 
    {
        if (urand(0, 1))
            DoScriptText(SAY_SLAY_1, m_creature);
        else
            DoScriptText(SAY_SLAY_2, m_creature);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_isInAimShotSequence)
        {
            if (!m_firstTrapThrown)
            {
                if (m_creature->GetHealthPercent() < 70.f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THROW_FREEZING_TRAP, TRIGGERED_NONE);
                    ResetTimer(MUSELEK_ACTION_DETERRENCE, GetInitialActionTimer(MUSELEK_ACTION_DETERRENCE));
                    m_firstTrapThrown = true;
                }
            }
            else if (!m_secondTrapThrown)
            {
                if (m_creature->GetHealthPercent() < 30.f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THROW_FREEZING_TRAP, TRIGGERED_NONE);
                    m_secondTrapThrown = true;
                }
            }

            UpdateTimers(diff, m_creature->IsInCombat());
            ExecuteActions();
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_swamplord_muselek(Creature* pCreature)
{
    return new boss_swamplord_muselekAI(pCreature);
}

void AddSC_boss_swamplord_muselek()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_swamplord_muselek";
    pNewScript->GetAI = &GetAI_boss_swamplord_muselek;
    pNewScript->RegisterSelf();
}
