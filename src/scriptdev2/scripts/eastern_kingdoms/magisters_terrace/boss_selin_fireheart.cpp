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
SD%Complete: 90
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "precompiled.h"
#include "magisters_terrace.h"

enum
{
    SAY_AGGRO                       = -1585000,
    SAY_ENERGY                      = -1585001,
    SAY_EMPOWERED                   = -1585002,
    SAY_KILL_1                      = -1585003,
    SAY_KILL_2                      = -1585004,
    SAY_DEATH                       = -1585005,
    EMOTE_CRYSTAL                   = -1585006,

    // Selin's spells
    SPELL_DRAIN_LIFE                = 44294,
    SPELL_DRAIN_LIFE_H              = 46155,
    SPELL_FEL_EXPLOSION             = 44314,
    SPELL_DRAIN_MANA                = 46153,                // Heroic only
    // SPELL_FEL_CRYSTAL_DUMMY       = 44329,               // used by Selin to select a nearby Crystal - not used in script
    SPELL_MANA_RAGE                 = 44320,                // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target

    // Crystal spells and npcs
    SPELL_FEL_CRYSTAL_COSMETIC      = 44374,                // cosmetic - used by the guys around Selin
    SPELL_FEL_CRYSTAL_VISUAL        = 44355,                // cosmetic

    NPC_HUSK                        = 24690,
    NPC_SKULER                      = 24688,
    NPC_BRUISER                     = 24689,
};

struct boss_selin_fireheartAI : public ScriptedAI
{
    boss_selin_fireheartAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_magisters_terrace*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_magisters_terrace* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDrainLifeTimer;
    uint32 m_uiDrainManaTimer;
    uint32 m_uiFelExplosionTimer;
    uint32 m_uiDrainCrystalTimer;
    uint32 m_uiManaRageTimer;

    bool m_bDrainingCrystal;

    ObjectGuid m_crystalGuid;

    void Reset() override
    {
        m_uiDrainLifeTimer    = urand(3000, 7000);
        m_uiDrainManaTimer    = m_uiDrainLifeTimer + 5000;
        m_uiFelExplosionTimer = 2100;
        m_uiDrainCrystalTimer = m_bIsRegularMode ? urand(20000, 25000) : urand(10000, 15000);
        m_uiManaRageTimer     = 0;

        m_bDrainingCrystal    = false;
    }

    // Get the closest alive crystal for draining
    bool DoSelectNearestCrystal()
    {
        // Wait to finish casting
        if (m_creature->IsNonMeleeSpellCasted(false))
            return false;

        if (Creature* pCrystal = GetClosestCreatureWithEntry(m_creature, NPC_FEL_CRYSTAL, 60.0f))
        {
            m_crystalGuid = pCrystal->GetObjectGuid();
            DoScriptText(SAY_ENERGY, m_creature);
            DoScriptText(EMOTE_CRYSTAL, m_creature);
            m_creature->InterruptNonMeleeSpells(false);

            float fX, fY, fZ;
            SetCombatMovement(false);
            m_creature->GetContactPoint(pCrystal, fX, fY, fZ, INTERACTION_DISTANCE);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            m_bDrainingCrystal = true;

            return true;
        }

        return false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        Creature* pCrystal = m_creature->GetMap()->GetCreature(m_crystalGuid);
        if (pCrystal && pCrystal->isAlive())
        {
            if (DoCastSpellIfCan(pCrystal, SPELL_MANA_RAGE) == CAST_OK)
            {
                // Allow the crystal to be killed
                pCrystal->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_uiManaRageTimer = 10000;
            }
        }
        else
        {
            // Make an error message in case something weird happened here
            script_error_log("Selin Fireheart unable to drain crystal as the crystal is either dead or deleted..");
            m_bDrainingCrystal = false;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SELIN, DONE);
    }

    // Mark the Mana Rage as complete
    void ManaRageComplete()
    {
        m_bDrainingCrystal = false;
        SetCombatMovement(true);
        m_creature->GetMotionMaster()->Clear();
        DoStartMovement(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bDrainingCrystal)
        {
            if (m_creature->GetPower(POWER_MANA) * 100 / m_creature->GetMaxPower(POWER_MANA) < 10)
            {
                if (m_uiDrainLifeTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_DRAIN_LIFE : SPELL_DRAIN_LIFE_H) == CAST_OK)
                            m_uiDrainLifeTimer = 10000;
                    }
                }
                else
                    m_uiDrainLifeTimer -= uiDiff;

                // Heroic only
                if (!m_bIsRegularMode)
                {
                    if (m_uiDrainManaTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DRAIN_MANA, SELECT_FLAG_POWER_MANA))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_DRAIN_MANA) == CAST_OK)
                                m_uiDrainManaTimer = 10000;
                        }
                    }
                    else
                        m_uiDrainManaTimer -= uiDiff;
                }

                if (m_uiDrainCrystalTimer < uiDiff)
                {
                    if (DoSelectNearestCrystal())
                        m_uiDrainCrystalTimer = m_bIsRegularMode ? urand(20000, 25000) : urand(10000, 15000);
                }
                else
                    m_uiDrainCrystalTimer -= uiDiff;
            }

            if (m_uiFelExplosionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FEL_EXPLOSION) == CAST_OK)
                    m_uiFelExplosionTimer = 2000;
            }
            else
                m_uiFelExplosionTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiManaRageTimer)
            {
                if (m_uiManaRageTimer <= uiDiff)
                {
                    DoScriptText(SAY_EMPOWERED, m_creature);
                    ManaRageComplete();

                    // Kill the drained crystal
                    Creature* pCrystalChosen = m_creature->GetMap()->GetCreature(m_crystalGuid);
                    if (pCrystalChosen && pCrystalChosen->isAlive())
                        pCrystalChosen->DealDamage(pCrystalChosen, pCrystalChosen->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                    m_uiManaRageTimer = 0;
                }
                else
                    m_uiManaRageTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_boss_selin_fireheart(Creature* pCreature)
{
    return new boss_selin_fireheartAI(pCreature);
};

struct mob_fel_crystalAI : public ScriptedAI
{
    mob_fel_crystalAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    GuidSet m_sWretchedGuids;

    uint32 m_uiVisualTimer;

    void Reset() override
    {
        m_uiVisualTimer = 1000;
        m_sWretchedGuids.clear();
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Cosmetic spell
        if (m_sWretchedGuids.find(pWho->GetObjectGuid()) == m_sWretchedGuids.end() && pWho->IsWithinDist(m_creature, 5.0f) && pWho->isAlive() &&
                (pWho->GetEntry() == NPC_SKULER || pWho->GetEntry() == NPC_BRUISER || pWho->GetEntry() == NPC_HUSK))
        {
            pWho->CastSpell(m_creature, SPELL_FEL_CRYSTAL_COSMETIC, false);
            m_sWretchedGuids.insert(pWho->GetObjectGuid());
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (ScriptedInstance* pInstance = (ScriptedInstance*)m_creature->GetInstanceData())
        {
            Creature* pSelin = pInstance->GetSingleCreatureFromStorage(NPC_SELIN_FIREHEART);
            if (!pSelin || !pSelin->isAlive())
                return;

            // Mark Mana rage as completed
            pSelin->InterruptNonMeleeSpells(false);
            if (boss_selin_fireheartAI* pBossAI = dynamic_cast<boss_selin_fireheartAI*>(pSelin->AI()))
                pBossAI->ManaRageComplete();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiVisualTimer)
        {
            if (m_uiVisualTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FEL_CRYSTAL_VISUAL) == CAST_OK)
                    m_uiVisualTimer = 0;
            }
            else
                m_uiVisualTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_fel_crystal(Creature* pCreature)
{
    return new mob_fel_crystalAI(pCreature);
};

void AddSC_boss_selin_fireheart()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_selin_fireheart";
    pNewScript->GetAI = &GetAI_boss_selin_fireheart;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_crystal";
    pNewScript->GetAI = &GetAI_mob_fel_crystal;
    pNewScript->RegisterSelf();
}
