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
SDName: boss_scourgelord_tyrannus
SD%Complete: 90
SDComment: Small adjustments may be required
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    SAY_AGGRO                           = -1658053,
    SAY_SLAY_1                          = -1658054,
    SAY_SLAY_2                          = -1658055,
    SAY_DEATH                           = -1658056,
    SAY_MARK                            = -1658057,
    SAY_SMASH                           = -1658058,

    EMOTE_RIMEFANG_ICEBOLT              = -1658059,
    EMOTE_SMASH                         = -1658060,

    // Tyrannus spells
    SPELL_FORCEFUL_SMASH                = 69155,
    SPELL_OVERLORDS_BRAND               = 69172,                // triggers 69189 and 69190 from target
    SPELL_UNHOLY_POWER                  = 69167,
    SPELL_MARK_OF_RIMEFANG              = 69275,

    // Rimefang spells
    SPELL_HOARFROST                     = 69246,
    SPELL_ICY_BLAST                     = 69232,
    SPELL_KILLING_ICE                   = 72531,

    // Icy blast
    // SPELL_ICY_BLAST_AURA             = 69238,
    NPC_ICY_BLAST                       = 36731,                // handled in eventAI
};

static const float afRimefangExitPos[3] = {1248.29f, 145.924f, 733.914f};

/*######
## boss_tyrannus
######*/

struct boss_tyrannusAI : public ScriptedAI
{
    boss_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        Reset();
    }

    instance_pit_of_saron* m_pInstance;

    uint32 m_uiForcefulSmashTimer;
    uint32 m_uiOverlordsBrandTimer;
    uint32 m_uiUnholyPowerTimer;
    uint32 m_uiMarkOfRimefangTimer;

    void Reset() override
    {
        m_uiForcefulSmashTimer  = 10000;
        m_uiOverlordsBrandTimer = 9000;
        m_uiUnholyPowerTimer    = urand(30000, 35000);
        m_uiMarkOfRimefangTimer = 20000;
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);

            // Set Rimefang in combat - ToDo: research if it has some wp movement during combat
            if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
                pRimefang->AI()->AttackStart(pWho);
        }
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_TYRANNUS, DONE);

            // Move Rimefang out of the area
            if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
            {
                pRimefang->AI()->EnterEvadeMode();
                pRimefang->SetWalk(false);
                pRimefang->ForcedDespawn(25000);
                pRimefang->GetMotionMaster()->MovePoint(0, afRimefangExitPos[0], afRimefangExitPos[1], afRimefangExitPos[2]);
            }

            // Move the general near the boss - ToDo: move the other freed slaves as well
            if (Creature* pGeneral = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == HORDE ? NPC_IRONSKULL_PART2 : NPC_VICTUS_PART2))
            {
                float fX, fY, fZ;
                pGeneral->SetWalk(false);
                m_creature->GetContactPoint(pGeneral, fX, fY, fZ, INTERACTION_DISTANCE);
                pGeneral->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
            }
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TYRANNUS, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiForcefulSmashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FORCEFUL_SMASH) == CAST_OK)
                m_uiForcefulSmashTimer = 50000;
        }
        else
            m_uiForcefulSmashTimer -= uiDiff;

        if (m_uiOverlordsBrandTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_OVERLORDS_BRAND) == CAST_OK)
                    m_uiOverlordsBrandTimer = urand(10000, 13000);
            }
        }
        else
            m_uiOverlordsBrandTimer -= uiDiff;

        if (m_uiUnholyPowerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_UNHOLY_POWER) == CAST_OK)
            {
                DoScriptText(SAY_SMASH, m_creature);
                DoScriptText(EMOTE_SMASH, m_creature);
                m_uiUnholyPowerTimer = 60000;
            }
        }
        else
            m_uiUnholyPowerTimer -= uiDiff;

        if (m_uiMarkOfRimefangTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_RIMEFANG) == CAST_OK)
                {
                    DoScriptText(SAY_MARK, m_creature);
                    if (m_pInstance)
                    {
                        if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
                        {
                            pRimefang->InterruptNonMeleeSpells(true);
                            pRimefang->CastSpell(pTarget, SPELL_HOARFROST, false);
                            DoScriptText(EMOTE_RIMEFANG_ICEBOLT, pRimefang, pTarget);
                        }
                    }
                    m_uiMarkOfRimefangTimer = urand(20000, 25000);
                }
            }
        }
        else
            m_uiMarkOfRimefangTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI(pCreature);
}

/*######
## boss_rimefang_pos
######*/

struct boss_rimefang_posAI : public ScriptedAI
{
    boss_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        m_bHasDoneIntro = false;
        m_uiMountTimer = 1000;
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    uint32 m_uiMountTimer;

    uint32 m_uiIcyBlastTimer;
    bool m_bHasDoneIntro;

    void Reset() override
    {
        m_uiIcyBlastTimer = 8000;
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(NULL);

        Reset();

        // Don't handle movement.
    }

    void AttackStart(Unit* pWho) override
    {
        // Don't attack unless Tyrannus is in combat or Ambush is completed
        if (m_pInstance && (m_pInstance->GetData(TYPE_AMBUSH) != DONE || m_pInstance->GetData(TYPE_TYRANNUS) != IN_PROGRESS))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_pInstance)
            return;

        // Check if ambush is done
        if (m_pInstance->GetData(TYPE_AMBUSH) != DONE)
            return;

        // Start the intro when possible
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 85.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            m_pInstance->SetData(TYPE_TYRANNUS, SPECIAL);
            m_bHasDoneIntro = true;
            return;
        }

        // Check for out of range players - ToDo: confirm the distance
        if (m_pInstance->GetData(TYPE_TYRANNUS) == IN_PROGRESS && pWho->GetTypeId() == TYPEID_PLAYER && !m_creature->IsWithinDistInMap(pWho, DEFAULT_VISIBILITY_INSTANCE))
            DoCastSpellIfCan(pWho, SPELL_KILLING_ICE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
            return;

        // He needs to be mounted manually, not by vehicle_accessories
        if (m_uiMountTimer)
        {
            if (m_uiMountTimer <= uiDiff)
            {
                if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS))
                    pTyrannus->CastSpell(m_creature, SPELL_RIDE_VEHICLE_HARDCODED, true);

                m_uiMountTimer = 0;
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiIcyBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ICY_BLAST) == CAST_OK)
                {
                    m_creature->SummonCreature(NPC_ICY_BLAST, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 90000);
                    m_uiIcyBlastTimer = 8000;
                }
            }
        }
        else
            m_uiIcyBlastTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_rimefang_pos(Creature* pCreature)
{
    return new boss_rimefang_posAI(pCreature);
}

void AddSC_boss_tyrannus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_tyrannus";
    pNewScript->GetAI = &GetAI_boss_tyrannus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_rimefang_pos";
    pNewScript->GetAI = &GetAI_boss_rimefang_pos;
    pNewScript->RegisterSelf();
}
