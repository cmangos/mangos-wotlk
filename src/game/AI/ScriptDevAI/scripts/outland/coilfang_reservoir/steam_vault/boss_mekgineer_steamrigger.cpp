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

#include "AI/ScriptDevAI/include/sc_common.h"
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
    SPELL_BERSERK               = 26662,

    NPC_STEAMRIGGER_MECHANIC    = 17951,

    SPELL_SUMMON_GNOME_1        = 31528,
    SPELL_SUMMON_GNOME_2        = 31529,
    SPELL_SUMMON_GNOME_3        = 31530,

    // Mechanic spells
    SPELL_DISPEL_MAGIC          = 17201,
    SPELL_REPAIR                = 31532,
    SPELL_REPAIR_H              = 37936,
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ, m_ori;
};

// Spawn locations used in spell_target_position
static const SummonLocation aSteamriggerSpawnLocs[] =
{
    { -316.101f, -166.444f, -7.66f, 2.5f }, // TODO: this orientation is guesswork
    { -348.497f, -161.719f, -7.66f, 0.3621517f },
    { -331.161f, -112.212f, -7.66f, 5.259035f },
};

static const uint32 gnomeSpells[] = { SPELL_SUMMON_GNOME_1, SPELL_SUMMON_GNOME_2, SPELL_SUMMON_GNOME_3 };

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
    uint32 m_uiBerserkTimer;
    uint8 m_uiMechanicPhaseCount;

    GuidVector m_spawns;

    void Reset() override
    {
        m_uiShrinkTimer         = 20000;
        m_uiSawBladeTimer       = 15000;
        m_uiElectrifiedNetTimer = 10000;
        m_uiMechanicTimer       = 20000;
        m_uiMechanicPhaseCount  = 1;
        m_uiBerserkTimer        = 300000;

        DespawnGuids(m_spawns);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void Aggro(Unit* /*who*/) override
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

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STEAMRIGGER_MECHANIC)
        {
            summoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
            m_spawns.push_back(summoned->GetObjectGuid());
        }
    }

    // Wrapper to summon three Mechanics
    void SummonMechanichs()
    {
        DoScriptText(SAY_MECHANICS, m_creature);

        m_creature->CastSpell(nullptr, SPELL_SUMMON_GNOME_1, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_GNOME_2, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_GNOME_3, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (!pTarget)
                pTarget = m_creature->GetVictim();

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
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
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
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
		
            if (m_uiMechanicTimer < uiDiff)
            {
                m_creature->CastSpell(nullptr, gnomeSpells[urand(0, 2)], TRIGGERED_OLD_TRIGGERED);
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
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_mekgineer_steamrigger()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_mekgineer_steamrigger";
    pNewScript->GetAI = &GetNewAIInstance<boss_mekgineer_steamriggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_steamrigger_mechanic";
    pNewScript->GetAI = &GetNewAIInstance<mob_steamrigger_mechanicAI>;
    pNewScript->RegisterSelf();
}
