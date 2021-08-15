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
SDName: Boss_Grand_Warlock_Nethekurse
SD%Complete: 75
SDComment: encounter not fully completed. missing part where boss kill minions.
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_grand_warlock_nethekurse
mob_fel_orc_convert
mob_lesser_shadow_fissure
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shattered_halls.h"

enum
{
    SAY_AGGRO_1         = -1540000, // all Fel Orc Converts alive
    SAY_AGGRO_2         = -1540001, // no Fel Orc Converts alive
    SAY_AGGRO_3         = -1540002, // 1-3 Fel Orc Converts alive?
    SAY_PEON_ATTACK_1   = -1540003,
    SAY_PEON_ATTACK_2   = -1540004,
    SAY_PEON_ATTACK_3   = -1540005,
    SAY_PEON_ATTACK_4   = -1540006,
    SAY_PEON_DIE_1      = -1540007,
    SAY_PEON_DIE_2      = -1540008,
    SAY_PEON_DIE_3      = -1540009,
    SAY_TAUNT_1         = -1540010,
    SAY_TAUNT_2         = -1540011,
    SAY_TAUNT_3         = -1540012,
    SAY_SLAY_1          = -1540013,
    SAY_SLAY_2          = -1540014,
    SAY_SLAY_3          = -1540015,
    SAY_SLAY_4          = -1540016,
    SAY_DIE             = -1540017,

    SPELL_DEATH_COIL       = 30500, // targets players
    SPELL_DEATH_COIL_2     = 30741, // targets all Fel Orc Converts (TAUNT_2)

    SPELL_DARK_SPIN        = 30502,

    SPELL_TARGET_FISSURES  = 30745, // somehow responsible for picking target of 30744
    SPELL_SHADOW_FISSURE   = 30496, // summons 17471 "Lesser Shadow Fissure" - targets players
    SPELL_SHADOW_FISSURE_2 = 30744, // summons 18370 "Wild Shadow Fissure" - targets Fel Orc Converts (TAUNT_3)

    SPELL_SHADOW_CLEAVE    = 30495,
    SPELL_SHADOW_SLAM_H    = 35953,

    SPELL_SHADOW_SEAR      = 30735, // targets 2 Fel Orc Converts (TAUNT_1)

    SPELL_HEMORRHAGE       = 30478,

    NPC_FEL_ORC_CONVERT    = 17083,
};

struct boss_grand_warlock_nethekurseAI : public ScriptedAI
{
    boss_grand_warlock_nethekurseAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIntroOnce = false;
        m_bIsIntroEvent = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIntroOnce;
    bool m_bIsIntroEvent;
    bool m_bIsMainEvent;
    bool m_bSpinOnce;
    bool m_firstPhase;

    uint8 m_uiPeonKilledCount;

    uint32 m_uiTauntTimer;
    uint32 m_uiDeathCoilTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiCleaveTimer;

    void Reset() override
    {
        m_bIsMainEvent = false;
        m_bSpinOnce = false;
        m_firstPhase = true;

        m_uiPeonKilledCount = 0;

        m_uiTauntTimer = 3000;
        m_uiDeathCoilTimer = 20000;
        m_uiShadowFissureTimer = 8000;
        m_uiCleaveTimer = 5000;

        SetCombatMovement(true);
    }

    void DoYellForPeonAggro(Unit* /*pWho*/)
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_PEON_ATTACK_1, m_creature); break;
            case 1: DoScriptText(SAY_PEON_ATTACK_2, m_creature); break;
            case 2: DoScriptText(SAY_PEON_ATTACK_3, m_creature); break;
            case 3: DoScriptText(SAY_PEON_ATTACK_4, m_creature); break;
        }
    }

    void DoYellForPeonDeath(Unit* pKiller)
    {
        if (m_uiPeonKilledCount >= 4)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_PEON_DIE_1, m_creature); break;
            case 1: DoScriptText(SAY_PEON_DIE_2, m_creature); break;
            case 2: DoScriptText(SAY_PEON_DIE_3, m_creature); break;
        }

        ++m_uiPeonKilledCount;

        if (m_uiPeonKilledCount == 4)
        {
            m_bIsIntroEvent = false;
            m_bIsMainEvent = true;

            if (pKiller)
                AttackStart(pKiller);
        }
    }

    void DoTauntPeons()
    {
        if (m_uiPeonKilledCount >= 4)
        {
            m_uiTauntTimer = 0;
            return;
        }

        std::list<Creature*> lFelConverts;
        GuidVector m_vFelConverts;

        GetCreatureListWithEntryInGrid(lFelConverts, m_creature, NPC_FEL_ORC_CONVERT, 40.0f);

        for (Creature* convert : lFelConverts)
            m_vFelConverts.push_back(convert->GetObjectGuid());

        if (m_vFelConverts.size() == 0)
            return;

        switch (urand(0, 2))
        {
            case 0:
                if (Creature* target = m_creature->GetMap()->GetCreature(m_vFelConverts[urand(0, m_vFelConverts.size() - 1)]))
                {
                    DoCastSpellIfCan(target, SPELL_SHADOW_SEAR);
                    DoScriptText(SAY_TAUNT_1, m_creature);
                }
                break;
            case 1:
                DoCastSpellIfCan(m_creature, SPELL_DEATH_COIL_2);
                DoScriptText(SAY_TAUNT_2, m_creature);
                break;
            case 2:
                //DoCastSpellIfCan(m_creature, SPELL_TARGET_FISSURES);
                if (Creature* target = m_creature->GetMap()->GetCreature(m_vFelConverts[urand(0, m_vFelConverts.size() - 1)]))
                {
                    DoCastSpellIfCan(target, SPELL_SHADOW_FISSURE_2);
                }
                DoScriptText(SAY_TAUNT_3, m_creature);
                break;
        }

        m_uiTauntTimer = urand(30000,35000);
    }

    // todo: use areatrigger 4347 instead (or when door lock is picked)
    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bIntroOnce && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() && m_creature->IsWithinDistInMap(pWho, 45.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            m_bIntroOnce = true;
            m_bIsIntroEvent = true;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_bIsIntroEvent = false;
        m_bIsMainEvent = true;
        switch (m_uiPeonKilledCount)
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: case 2: case 3: DoScriptText(SAY_AGGRO_3, m_creature); break;
            case 4: DoScriptText(SAY_AGGRO_2, m_creature); break;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DIE, m_creature);

        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_NETHEKURSE, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHEKURSE, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsIntroEvent)
        {
            if (!m_pInstance)
                return;

            if (m_pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
            {
                if (m_uiTauntTimer < uiDiff)
                    DoTauntPeons();
                else
                    m_uiTauntTimer -= uiDiff;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bIsMainEvent)
            return;

        if (m_firstPhase)
        {
            if (m_uiCleaveTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_SHADOW_CLEAVE : SPELL_SHADOW_SLAM_H);
                m_uiCleaveTimer = urand(6000, 8500);
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiShadowFissureTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(pTarget, SPELL_SHADOW_FISSURE);
                m_uiShadowFissureTimer = urand(7500, 15000);
            }
            else
                m_uiShadowFissureTimer -= uiDiff;

            if (m_uiDeathCoilTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(pTarget, SPELL_DEATH_COIL);
                m_uiDeathCoilTimer = urand(15000, 20000);
            }
            else
                m_uiDeathCoilTimer -= uiDiff;

            if (m_creature->GetHealthPercent() <= 20.0f)
                m_firstPhase = false;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (!m_bSpinOnce)
            {
                SetCombatMovement(false);
                DoCastSpellIfCan(nullptr, SPELL_DARK_SPIN);
                m_bSpinOnce = true;
            }
        }
    }
};

static const int32 aRandomAggro[] = { -1540200, -1540201, -1540202, -1540203, -1540204, -1540205, -1540206 };

struct mob_fel_orc_convertAI : public ScriptedAI
{
    mob_fel_orc_convertAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiHemorrhageTimer;

    void Reset() override
    {
        m_uiHemorrhageTimer = 3000;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && !((Player*) pWho)->IsGameMaster() && m_creature->IsWithinDistInMap(pWho, 20.0f) && m_creature->IsWithinLOSInMap(pWho))
            m_creature->SetInCombatWithZone();
    }

    void AttackedBy(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_NETHEKURSE)
            return;

        ScriptedAI::AttackedBy(pWho);
    }

    void Aggro(Unit* pWho) override
    {
        if (urand(0, 4) == 4)
            DoScriptText(aRandomAggro[urand(0, 6)], m_creature);

        if (m_pInstance)
        {
            Creature* pKurse = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHEKURSE);
            if (pKurse && m_creature->IsWithinDist(pKurse, 45.0f))
            {
                if (boss_grand_warlock_nethekurseAI* pKurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(pKurse->AI()))
                    pKurseAI->DoYellForPeonAggro(pWho);

                if (m_pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                    return;
                m_pInstance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
            }
        }
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_NETHEKURSE) != IN_PROGRESS)
                return;

            if (Creature* pKurse = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHEKURSE))
            {
                if (boss_grand_warlock_nethekurseAI* pKurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(pKurse->AI()))
                    pKurseAI->DoYellForPeonDeath(pKiller);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiHemorrhageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEMORRHAGE);
            m_uiHemorrhageTimer = 15000;
        }
        else
            m_uiHemorrhageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_grand_warlock_nethekurse(Creature* pCreature)
{
    return new boss_grand_warlock_nethekurseAI(pCreature);
}

UnitAI* GetAI_mob_fel_orc_convert(Creature* pCreature)
{
    return new mob_fel_orc_convertAI(pCreature);
}

void AddSC_boss_grand_warlock_nethekurse()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_grand_warlock_nethekurse";
    pNewScript->GetAI = &GetAI_boss_grand_warlock_nethekurse;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_orc_convert";
    pNewScript->GetAI = &GetAI_mob_fel_orc_convert;
    pNewScript->RegisterSelf();
}
