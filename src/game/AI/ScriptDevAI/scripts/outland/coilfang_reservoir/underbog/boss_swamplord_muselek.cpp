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
#include "AI/ScriptDevAI/base/CombatAI.h"

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
    MUSELEK_TRAP_ONE,
    MUSELEK_TRAP_TWO,
    MUSELEK_AIMED_SHOT,
    MUSELEK_ACTION_KNOCK_AWAY,
    MUSELEK_ACTION_RAPTOR_STRIKE,
    MUSELEK_ACTION_BEAR_COMMAND,
    MUSELEK_ACTION_RANGED_ATTACK,
    MUSELEK_ACTION_DETERRENCE,
    MUSELEK_ACTION_MAX,
};

struct boss_swamplord_muselekAI : public CombatAI
{
    boss_swamplord_muselekAI(Creature* creature) : CombatAI(creature, MUSELEK_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(MUSELEK_TRAP_ONE, true);
        AddTimerlessCombatAction(MUSELEK_TRAP_TWO, true);
        AddTimerlessCombatAction(MUSELEK_AIMED_SHOT, false);
        AddCombatAction(MUSELEK_ACTION_KNOCK_AWAY, 25000, 30000);
        AddCombatAction(MUSELEK_ACTION_RAPTOR_STRIKE, 1500, 4000);
        AddCombatAction(MUSELEK_ACTION_BEAR_COMMAND, 8000, 12000);
        AddCombatAction(MUSELEK_ACTION_RANGED_ATTACK, 500, 1250);
        AddCombatAction(MUSELEK_ACTION_DETERRENCE, true);
        SetRangedMode(true, 30.f, TYPE_PROXIMITY);
        AddMainSpell(SPELL_SHOOT);
        AddMainSpell(SPELL_MULTI_SHOT);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
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

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MUSELEK_TRAP_ONE:
            {
                if (m_creature->GetHealthPercent() < 70.f)
                {
                    DoCastSpellIfCan(m_creature->GetVictim(), SPELL_THROW_FREEZING_TRAP, TRIGGERED_NONE);
                    DoCastSpellIfCan(nullptr, SPELL_HUNTERS_MARK);
                    DistanceYourself();
                    ResetTimer(MUSELEK_ACTION_DETERRENCE, urand(15000, 20000));
                    SetActionReadyStatus(action, false);
                }
                break;
            }
            case MUSELEK_TRAP_TWO:
            {
                if (m_creature->GetHealthPercent() < 30.f)
                {
                    DoCastSpellIfCan(m_creature->GetVictim(), SPELL_THROW_FREEZING_TRAP, TRIGGERED_NONE);
                    DoCastSpellIfCan(nullptr, SPELL_HUNTERS_MARK);
                    DistanceYourself();
                    SetActionReadyStatus(action, false);
                }
                break;
            }
            case MUSELEK_AIMED_SHOT:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_AIMED_SHOT) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            }
            case MUSELEK_ACTION_KNOCK_AWAY:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_KNOCK_AWAY) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MUSELEK_ACTION_RAPTOR_STRIKE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAPTOR_STRIKE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MUSELEK_ACTION_BEAR_COMMAND:
            {
                if (Creature* claw = m_instance->GetSingleCreatureFromStorage(NPC_CLAW))
                    if (claw->IsAlive() && claw->GetEntry() != NPC_CLAW_DRUID_FORM)
                    {
                        uint8 claw_spell = urand(0, 2);

                        switch (claw_spell)
                        {
                            case 0:
                                claw->AI()->DoCastSpellIfCan(claw, SPELL_MAUL);
                                break;
                            case 1:
                                claw->AI()->DoCastSpellIfCan(claw, SPELL_ROAR);
                                break;
                            default:
                                break;
                        }

                        claw->AI()->DoCastSpellIfCan(claw, SPELL_FRENZY);
                        DoScriptText(SAY_COMMAND, m_creature);
                    }

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MUSELEK_ACTION_RANGED_ATTACK:
            {
                if (!GetCurrentRangedMode())
                    return;

                uint32 RangedSpell = urand(0, 4) ? SPELL_SHOOT : SPELL_MULTI_SHOT; // 66% shoot, 33% multishot
                if (DoCastSpellIfCan(m_creature->GetVictim(), RangedSpell) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case MUSELEK_ACTION_DETERRENCE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DETERRENCE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
        }
    }
    
    void DistancingEnded() override
    {
        CombatAI::DistancingEnded();
        ResetCombatAction(MUSELEK_ACTION_RANGED_ATTACK, GetSubsequentActionTimer(MUSELEK_ACTION_RANGED_ATTACK));
        SetActionReadyStatus(MUSELEK_AIMED_SHOT, false); // currently never used due to not being interrupted when the target is in his melee range (oneshotting everything)
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell) override 
    {
        if (spell->Id == SPELL_FREEZING_TRAP_EFFECT)
            if (Creature *claw = m_instance->GetSingleCreatureFromStorage(NPC_CLAW))
                if (claw->IsAlive() && claw->GetEntry() != NPC_CLAW_DRUID_FORM)
                    claw->getThreatManager().modifyThreatPercent(target, (0 - urand(30, 99))); // Freezing trap messes with bear aggro
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
        if (Creature* claw = m_instance->GetSingleCreatureFromStorage(NPC_CLAW))
            m_creature->CastSpell(claw, SPELL_NOTIFY_OF_DEATH, TRIGGERED_NONE); // TODO: what does this do?
        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override 
    {
        if (Creature *claw = m_instance->GetSingleCreatureFromStorage(NPC_CLAW))
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
};

void AddSC_boss_swamplord_muselek()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_swamplord_muselek";
    pNewScript->GetAI = &GetNewAIInstance<boss_swamplord_muselekAI>;
    pNewScript->RegisterSelf();
}
