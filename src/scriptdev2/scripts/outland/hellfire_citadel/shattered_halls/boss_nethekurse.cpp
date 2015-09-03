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

#include "precompiled.h"
#include "shattered_halls.h"

struct Say
{
    int32 id;
};

static Say PeonAttacked[] =
{
    { -1540001},
    { -1540002},
    { -1540003},
    { -1540004},
};

static Say PeonDies[] =
{
    { -1540005},
    { -1540006},
    { -1540007},
    { -1540008},
};

enum
{
    SAY_INTRO              = -1540000,
    SAY_TAUNT_1            = -1540009,
    SAY_TAUNT_2            = -1540010,
    SAY_TAUNT_3            = -1540011,
    SAY_AGGRO_1            = -1540012,
    SAY_AGGRO_2            = -1540013,
    SAY_AGGRO_3            = -1540014,
    SAY_SLAY_1             = -1540015,
    SAY_SLAY_2             = -1540016,
    SAY_DIE                = -1540017,

    SPELL_DEATH_COIL       = 30500,
    SPELL_DARK_SPIN        = 30502,                         // core bug spell attack caster :D
    SPELL_SHADOW_FISSURE   = 30496,                         // Summon the ShadowFissure NPC

    SPELL_SHADOW_CLEAVE    = 30495,
    SPELL_SHADOW_SLAM_H    = 35953,

    SPELL_SHADOW_SEAR      = 30735,                         // On fel orcs - not sure yet how it is used
    SPELL_HEMORRHAGE       = 30478,

    SPELL_CONSUMPTION      = 30497,                         // Cast by the shadow fissure

    NPC_FEL_ORC_CONVERT    = 17083,
};

struct boss_grand_warlock_nethekurseAI : public ScriptedAI
{
    boss_grand_warlock_nethekurseAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIntroOnce = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIntroOnce;
    bool m_bIsIntroEvent;
    bool m_bIsMainEvent;
    bool m_bSpinOnce;
    // bool m_bHasTaunted;
    bool m_bPhase;

    uint32 m_uiPeonEngagedCount;
    uint32 m_uiPeonKilledCount;

    uint32 m_uiIntroEventTimer;
    uint32 m_uiDeathCoilTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiCleaveTimer;

    ObjectGuid m_lastEventInvokerGuid;

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_bIsIntroEvent = false;
        m_bIsMainEvent = false;
        // m_bHasTaunted = false;
        m_bSpinOnce = false;
        m_bPhase = false;

        m_uiPeonEngagedCount = 0;
        m_uiPeonKilledCount = 0;

        m_uiIntroEventTimer = 90000;                        // how long before getting bored and kills his minions?
        m_uiDeathCoilTimer = 20000;
        m_uiShadowFissureTimer = 8000;
        m_uiCleaveTimer = 5000;

        m_lastEventInvokerGuid.Clear();
    }

    void DoYellForPeonAggro(Unit* pWho)
    {
        if (m_uiPeonEngagedCount >= 4)
            return;

        DoScriptText(PeonAttacked[m_uiPeonEngagedCount].id, m_creature);
        ++m_uiPeonEngagedCount;

        if (pWho)
            m_lastEventInvokerGuid = pWho->GetObjectGuid();
    }

    void DoYellForPeonDeath(Unit* pKiller)
    {
        if (m_uiPeonKilledCount >= 4)
            return;

        DoScriptText(PeonDies[m_uiPeonKilledCount].id, m_creature);
        ++m_uiPeonKilledCount;

        if (m_uiPeonKilledCount == 4)
        {
            m_bIsIntroEvent = false;
            m_bIsMainEvent = true;
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (pKiller)
                AttackStart(pKiller);
        }
    }

    void DoTauntPeons()
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_TAUNT_1, m_creature); break;
            case 1: DoScriptText(SAY_TAUNT_2, m_creature); break;
            case 2: DoScriptText(SAY_TAUNT_3, m_creature); break;
        }

        std::list<Creature*> lFelConverts;
        GetCreatureListWithEntryInGrid(lFelConverts, m_creature, NPC_FEL_ORC_CONVERT, 40.0f);
        for (std::list<Creature*>::iterator itr = lFelConverts.begin(); itr != lFelConverts.end(); ++itr)
            (*itr)->DealDamage(*itr, (*itr)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        m_bIsIntroEvent = false;
        m_uiPeonEngagedCount = 4;
        m_uiPeonKilledCount = 4;
        m_bIsMainEvent = true;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (Unit* pEnemy = m_creature->GetMap()->GetUnit(m_lastEventInvokerGuid))
            AttackStart(pEnemy);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_bIsIntroEvent || !m_bIsMainEvent)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (m_bPhase)
                DoStartNoMovement(pWho);
            else
                DoStartMovement(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bIntroOnce && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 50.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bIntroOnce = true;
            m_bIsIntroEvent = true;

            m_lastEventInvokerGuid = pWho->GetObjectGuid();

            if (m_pInstance)
                m_pInstance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
        }

        if (m_bIsIntroEvent || !m_bIsMainEvent)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // ToDo: this should be done in DB
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        pSummoned->CastSpell(pSummoned, SPELL_CONSUMPTION, false, NULL, NULL, m_creature->GetObjectGuid());
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
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

        std::list<Creature*> lFelConverts;
        GetCreatureListWithEntryInGrid(lFelConverts, m_creature, NPC_FEL_ORC_CONVERT, 40.0f);
        for (std::list<Creature*>::iterator itr = lFelConverts.begin(); itr != lFelConverts.end(); ++itr)
        {
            if (!(*itr)->isAlive())
                (*itr)->Respawn();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsIntroEvent)
        {
            if (!m_pInstance)
                return;

            if (m_pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
            {
                if (m_uiIntroEventTimer < uiDiff)
                    DoTauntPeons();
                else
                    m_uiIntroEventTimer -= uiDiff;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsMainEvent)
            return;

        if (m_bPhase)
        {
            if (!m_bSpinOnce)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_SPIN);
                m_bSpinOnce = true;
            }

            if (m_uiCleaveTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_CLEAVE : SPELL_SHADOW_SLAM_H);
                m_uiCleaveTimer = urand(6000, 8500);
            }
            else
                m_uiCleaveTimer -= uiDiff;
        }
        else
        {
            if (m_uiShadowFissureTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCastSpellIfCan(pTarget, SPELL_SHADOW_FISSURE);
                m_uiShadowFissureTimer = urand(7500, 15000);
            }
            else
                m_uiShadowFissureTimer -= uiDiff;

            if (m_uiDeathCoilTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCastSpellIfCan(pTarget, SPELL_DEATH_COIL);
                m_uiDeathCoilTimer = urand(15000, 20000);
            }
            else
                m_uiDeathCoilTimer -= uiDiff;

            if (m_creature->GetHealthPercent() <= 20.0f)
                m_bPhase = true;

            DoMeleeAttackIfReady();
        }
    }
};

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
        m_creature->SetNoCallAssistance(true);              // we don't want any assistance (WE R HEROZ!)
        m_uiHemorrhageTimer = 3000;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override
    {
        return;
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
        {
            Creature* pKurse = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHEKURSE);
            if (pKurse && m_creature->IsWithinDist(pKurse, 45.0f))
            {
                if (boss_grand_warlock_nethekurseAI* pKurseAI = dynamic_cast<boss_grand_warlock_nethekurseAI*>(pKurse->AI()))
                    pKurseAI->DoYellForPeonAggro(pWho);

                if (m_pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                    return;
                else
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
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHemorrhageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEMORRHAGE);
            m_uiHemorrhageTimer = 15000;
        }
        else
            m_uiHemorrhageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// NOTE: this creature are also summoned by other spells, for different creatures
struct mob_lesser_shadow_fissureAI : public Scripted_NoMovementAI
{
    mob_lesser_shadow_fissureAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
};

CreatureAI* GetAI_boss_grand_warlock_nethekurse(Creature* pCreature)
{
    return new boss_grand_warlock_nethekurseAI(pCreature);
}

CreatureAI* GetAI_mob_fel_orc_convert(Creature* pCreature)
{
    return new mob_fel_orc_convertAI(pCreature);
}

CreatureAI* GetAI_mob_lesser_shadow_fissure(Creature* pCreature)
{
    return new mob_lesser_shadow_fissureAI(pCreature);
}

void AddSC_boss_grand_warlock_nethekurse()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_grand_warlock_nethekurse";
    pNewScript->GetAI = &GetAI_boss_grand_warlock_nethekurse;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_orc_convert";
    pNewScript->GetAI = &GetAI_mob_fel_orc_convert;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_lesser_shadow_fissure";
    pNewScript->GetAI = &GetAI_mob_lesser_shadow_fissure;
    pNewScript->RegisterSelf();
}
