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
SDName: Boss_Mekgineer_Steamrigger
SD%Complete: 80
SDComment: Enrage on heroic NYI
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_mekgineer_steamrigger
mob_steamrigger_mechanic
EndContentData */

#include "precompiled.h"
#include "steam_vault.h"

enum
{
    SAY_MECHANICS               = -1545007,
    SAY_AGGRO_1                 = -1545008,
    SAY_AGGRO_2                 = -1545009,
    SAY_AGGRO_3                 = -1545010,
    SAY_AGGRO_4                 = -1545011,
    SAY_SLAY_1                  = -1545012,
    SAY_SLAY_2                  = -1545013,
    SAY_SLAY_3                  = -1545014,
    SAY_DEATH                   = -1545015,

    SPELL_SUPER_SHRINK_RAY      = 31485,
    SPELL_SAW_BLADE             = 31486,
    SPELL_ELECTRIFIED_NET       = 35107,
    // SPELL_ENRAGE_H            = 1,                       // current enrage spell not known

    NPC_STEAMRIGGER_MECHANIC    = 17951,

    // Mechanic spells
    SPELL_DISPEL_MAGIC          = 17201,
    SPELL_REPAIR                = 31532,
    SPELL_REPAIR_H              = 37936,
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ;
};

// Spawn locations
static const SummonLocation aSteamriggerSpawnLocs[] =
{
    { -316.101f, -166.444f, -7.66f},
    { -348.497f, -161.718f, -7.66f},
    { -331.161f, -112.212f, -7.66f},
};

struct boss_mekgineer_steamriggerAI : public ScriptedAI
{
    boss_mekgineer_steamriggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShrinkTimer;
    uint32 m_uiSawBladeTimer;
    uint32 m_uiElectrifiedNetTimer;
    uint32 m_uiMechanicTimer;
    uint8 m_uiMechanicPhaseCount;

    void Reset() override
    {
        m_uiShrinkTimer         = 20000;
        m_uiSawBladeTimer       = 15000;
        m_uiElectrifiedNetTimer = 10000;
        m_uiMechanicTimer       = 20000;
        m_uiMechanicPhaseCount  = 1;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_STEAMRIGGER_MECHANIC)
            pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
    }

    // Wrapper to summon three Mechanics
    void SummonMechanichs()
    {
        DoScriptText(SAY_MECHANICS, m_creature);

        for (uint8 i = 0; i < 3; ++i)
            m_creature->SummonCreature(NPC_STEAMRIGGER_MECHANIC, aSteamriggerSpawnLocs[i].m_fX, aSteamriggerSpawnLocs[i].m_fY, aSteamriggerSpawnLocs[i].m_fZ, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 240000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShrinkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUPER_SHRINK_RAY) == CAST_OK)
                m_uiShrinkTimer = 20000;
        }
        else
            m_uiShrinkTimer -= uiDiff;

        if (m_uiSawBladeTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if (!pTarget)
                pTarget = m_creature->getVictim();

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SAW_BLADE) == CAST_OK)
                    m_uiSawBladeTimer = 15000;
            }
        }
        else
            m_uiSawBladeTimer -= uiDiff;

        if (m_uiElectrifiedNetTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ELECTRIFIED_NET) == CAST_OK)
                    m_uiElectrifiedNetTimer = 10000;
            }
        }
        else
            m_uiElectrifiedNetTimer -= uiDiff;

        // On Heroic mode summon a mechanic at each 20 secs
        if (!m_bIsRegularMode)
        {
            if (m_uiMechanicTimer < uiDiff)
            {
                m_creature->SummonCreature(NPC_STEAMRIGGER_MECHANIC, aSteamriggerSpawnLocs[2].m_fX, aSteamriggerSpawnLocs[2].m_fY, aSteamriggerSpawnLocs[2].m_fZ, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 240000);
                m_uiMechanicTimer = 20000;
            }
            else
                m_uiMechanicTimer -= uiDiff;
        }

        if (m_creature->GetHealthPercent() < (100 - 25 * m_uiMechanicPhaseCount))
        {
            SummonMechanichs();
            ++m_uiMechanicPhaseCount;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mekgineer_steamrigger(Creature* pCreature)
{
    return new boss_mekgineer_steamriggerAI(pCreature);
}

struct mob_steamrigger_mechanicAI : public ScriptedAI
{
    mob_steamrigger_mechanicAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bCanStartAttack;

    void Reset() override
    {
        m_bCanStartAttack = false;
    }

    void AttackStart(Unit* pWho) override
    {
        // Trigger attack only for players
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        m_creature->InterruptNonMeleeSpells(false);
        ScriptedAI::AttackStart(pWho);
        m_bCanStartAttack = true;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Return if already in combat
        if (m_bCanStartAttack)
            return;

        // Don't attack players unless attacked
        if (pWho->GetEntry() == NPC_STEAMRIGGER)
        {
            if (m_pInstance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
            {
                // Channel the repair spell on Steamrigger
                // This will also stop creature movement and will allow them to continue to follow the boss after channeling is finished or the boss is out of range
                if (m_creature->IsWithinDistInMap(pWho, 2 * INTERACTION_DISTANCE))
                    DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_REPAIR : SPELL_REPAIR_H);
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_steamrigger_mechanic(Creature* pCreature)
{
    return new mob_steamrigger_mechanicAI(pCreature);
}

void AddSC_boss_mekgineer_steamrigger()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mekgineer_steamrigger";
    pNewScript->GetAI = &GetAI_boss_mekgineer_steamrigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_steamrigger_mechanic";
    pNewScript->GetAI = &GetAI_mob_steamrigger_mechanic;
    pNewScript->RegisterSelf();
}
