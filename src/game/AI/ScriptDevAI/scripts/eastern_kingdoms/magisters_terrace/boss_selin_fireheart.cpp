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
SDName: Boss_Selin_Fireheart
SD%Complete: 95
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "magisters_terrace.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_AGGRO                       = -1585000,
    SAY_DRAIN_1                     = -1585001, // My hunger knows no bounds!
    SAY_EMPOWERED                   = -1585002,
    SAY_KILL_NORMAL                 = -1585003, // Enough distractions!
    SAY_KILL_EMPOWERED              = -1585004, // I am invincible!
    SAY_DRAIN_2                     = -1585005, // No! More... I must have more!
    EMOTE_CRYSTAL                   = -1585006, // %s begins to channel from the nearby Fel Crystal...

    // Selin's spells
    SPELL_DRAIN_LIFE                = 44294,
    SPELL_DRAIN_LIFE_H              = 46155,
    SPELL_FEL_EXPLOSION             = 44314,
    SPELL_DRAIN_MANA                = 46153,                // Heroic only
    //SPELL_FEL_CRYSTAL_DUMMY        = 44329,               // used by Selin to select a nearby Crystal - not used in script
    SPELL_MANA_RAGE_CHANNEL         = 44320,                // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target
    //SPELL_MANA_RAGE_POWER          = 44321,               // Triggered by the channel

    // Crystal spells
    //SPELL_FEL_CRYSTAL_VISUAL       = 44355,                // cosmetic - defined in instance header
};

enum SelinActions
{
    SELIN_ACTION_FEL_EXPLOSION,
    SELIN_ACTION_DRAIN_LIFE,
    SELIN_ACTION_DRAIN_MANA,
    SELIN_ACTION_DRAIN_CRYSTAL,
    SELIN_ACTION_MAX,
};

struct boss_selin_fireheartAI : public ScriptedAI, public CombatActions
{
    boss_selin_fireheartAI(Creature* pCreature) : ScriptedAI(pCreature), CombatActions(SELIN_ACTION_MAX)
    {
        m_pInstance = (instance_magisters_terrace*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        AddCombatAction(SELIN_ACTION_FEL_EXPLOSION, 0u);
        AddCombatAction(SELIN_ACTION_DRAIN_LIFE, 0u);
        AddCombatAction(SELIN_ACTION_DRAIN_MANA, 0u);
        AddCombatAction(SELIN_ACTION_DRAIN_CRYSTAL, 0u);
        Reset();
    }

    instance_magisters_terrace* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bDrainingCrystal;
    bool m_bEmpowered;

    uint32 m_uiVisualTimer;

    ObjectGuid m_crystalGuid;

    void Reset() override
    {
        for (uint32 i = 0; i < SELIN_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(SELIN_ACTION_FEL_EXPLOSION, GetInitialActionTimer(SELIN_ACTION_FEL_EXPLOSION));
        ResetTimer(SELIN_ACTION_DRAIN_LIFE, GetInitialActionTimer(SELIN_ACTION_DRAIN_LIFE));
        ResetTimer(SELIN_ACTION_DRAIN_MANA, GetInitialActionTimer(SELIN_ACTION_DRAIN_MANA));
        ResetTimer(SELIN_ACTION_DRAIN_CRYSTAL, GetInitialActionTimer(SELIN_ACTION_DRAIN_CRYSTAL));

        m_uiVisualTimer = 3000;

        m_bDrainingCrystal = false;
        m_bEmpowered = false;
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case SELIN_ACTION_FEL_EXPLOSION: return 2100;
            case SELIN_ACTION_DRAIN_LIFE: return urand(30000, 45000);
            case SELIN_ACTION_DRAIN_MANA: return 10000;
            case SELIN_ACTION_DRAIN_CRYSTAL: return urand(15000, 25000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case SELIN_ACTION_FEL_EXPLOSION: return 2000;
            case SELIN_ACTION_DRAIN_LIFE: return urand(12000, 22000);
            case SELIN_ACTION_DRAIN_MANA: return urand(17500, 25000);
            case SELIN_ACTION_DRAIN_CRYSTAL: return m_bIsRegularMode ? urand(40000, 55000) : urand(30000, 40000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < SELIN_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case SELIN_ACTION_FEL_EXPLOSION:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_FEL_EXPLOSION);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case SELIN_ACTION_DRAIN_LIFE:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DRAIN_LIFE, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_DRAIN_LIFE : SPELL_DRAIN_LIFE_H);

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case SELIN_ACTION_DRAIN_MANA:
                    {
                        if (!m_bIsRegularMode)
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DRAIN_MANA, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                                DoCastSpellIfCan(pTarget, SPELL_DRAIN_MANA);
                        }

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case SELIN_ACTION_DRAIN_CRYSTAL:
                    {
                        if (DoSelectNearestCrystal())
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                }
            }
        }
    }

    // Get the nearest alive crystal for draining
    bool DoSelectNearestCrystal()
    {
        // Wait to finish casting
        if (m_creature->IsNonMeleeSpellCasted(false))
            return false;

        if (Creature* pCrystal = GetClosestCreatureWithEntry(m_creature, NPC_FEL_CRYSTAL, 60.0f))
        {
            m_crystalGuid = pCrystal->GetObjectGuid();
            DoScriptText(urand(0, 1) ? SAY_DRAIN_1 : SAY_DRAIN_2, m_creature);
            m_creature->InterruptNonMeleeSpells(false);

            float fX, fY, fZ;
            m_creature->GetContactPoint(pCrystal, fX, fY, fZ, INTERACTION_DISTANCE);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            m_bDrainingCrystal = true;

            return true;
        }

        return false;
    }

    void DoEndCrystalDraining()
    {
        m_bDrainingCrystal = false;
        m_creature->GetMotionMaster()->Clear();
        DoStartMovement(m_creature->getVictim());
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, IN_PROGRESS);
    }

    void JustRespawned() override
    {
        if (m_pInstance)
            m_pInstance->StartCrystalVisual();
    }

    void JustReachedHome() override
    {
        m_creature->SetPower(POWER_MANA, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(m_bEmpowered ? SAY_KILL_EMPOWERED : SAY_KILL_NORMAL, m_creature);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        bool castSuccessful = true;
        Creature* pCrystal = m_creature->GetMap()->GetCreature(m_crystalGuid);
        if (pCrystal && pCrystal->isAlive())
        {
            //if (DoCastSpellIfCan(pCrystal, SPELL_MANA_RAGE_CHANNEL) == CAST_OK)
            if (pCrystal->CastSpell(nullptr, SPELL_MANA_RAGE_CHANNEL, TRIGGERED_NONE) == SPELL_CAST_OK)
            {
                // Inform Crystal it is being drained
                pCrystal->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pCrystal);
                DoScriptText(EMOTE_CRYSTAL, m_creature);
            }
            else
                castSuccessful = false;
        }
        else
            castSuccessful = false;

        if (!castSuccessful)
        {
            // Make an error message in case something weird happened here
            script_error_log("Selin Fireheart unable to drain crystal as the crystal is either dead or deleted..");
            DoEndCrystalDraining(); // Just in case
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, DONE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A: // Channeling Completed
                DoScriptText(SAY_EMPOWERED, m_creature);
                m_bEmpowered = true;
            case AI_EVENT_JUST_DIED: // Channeled Crystal Died
                DoEndCrystalDraining();
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bDrainingCrystal)
        {
            UpdateTimers(uiDiff, m_creature->isInCombat());
            ExecuteActions();

            DoMeleeAttackIfReady();
        }
    }
};

UnitAI* GetAI_boss_selin_fireheart(Creature* pCreature)
{
    return new boss_selin_fireheartAI(pCreature);
};

struct mob_fel_crystalAI : public ScriptedAI
{
    mob_fel_crystalAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override 
    {
        m_uiChannelTime = 0;
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void MoveInLineOfSight(Unit* /*pWho*/) override {}

    uint32 m_uiChannelTime;

    void JustRespawned() override
    {
        m_uiChannelTime = 0;
        m_creature->CastSpell(nullptr, SPELL_FEL_CRYSTAL_VISUAL, TRIGGERED_NONE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (ScriptedInstance* pInstance = (ScriptedInstance*)m_creature->GetInstanceData())
        {
            Creature* pSelin = pInstance->GetSingleCreatureFromStorage(NPC_SELIN_FIREHEART);
            if (!pSelin || !pSelin->isAlive())
                return;

            // Mark Mana rage as completed
            pSelin->AI()->SendAIEvent(AI_EVENT_JUST_DIED, m_creature, pSelin);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A: // You are being drained
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_uiChannelTime = 10000;
                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiChannelTime)
        {
            if (m_uiChannelTime < uiDiff)
            {
                if (ScriptedInstance* pInstance = (ScriptedInstance*)m_creature->GetInstanceData())
                {
                    if (Creature* pSelin = pInstance->GetSingleCreatureFromStorage(NPC_SELIN_FIREHEART))
                        pSelin->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pSelin); // Inform Selin channeling finished
                }

                if (m_creature->isAlive()) // Kill self
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);

                m_uiChannelTime = 0; // Just in case something goes wrong
            }
            else
                m_uiChannelTime -= uiDiff;
        }
    }
};

UnitAI* GetAI_mob_fel_crystal(Creature* pCreature)
{
    return new mob_fel_crystalAI(pCreature);
};

void AddSC_boss_selin_fireheart()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_selin_fireheart";
    pNewScript->GetAI = &GetAI_boss_selin_fireheart;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_crystal";
    pNewScript->GetAI = &GetAI_mob_fel_crystal;
    pNewScript->RegisterSelf();
}
