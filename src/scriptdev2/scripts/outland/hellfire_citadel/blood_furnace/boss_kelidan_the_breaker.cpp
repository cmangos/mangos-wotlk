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
SDName: Boss_Kelidan_The_Breaker
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

/* ContentData
boss_kelidan_the_breaker
mob_shadowmoon_channeler
EndContentData */

#include "precompiled.h"
#include "blood_furnace.h"

enum
{
    MAX_ADDS                    = 5,

    SAY_MAGTHERIDON_INTRO       = -1542016,                 // Yell by Magtheridon
    SAY_WAKE                    = -1542000,
    SAY_ADD_AGGRO_1             = -1542001,
    SAY_ADD_AGGRO_2             = -1542002,
    SAY_ADD_AGGRO_3             = -1542003,
    SAY_KILL_1                  = -1542004,
    SAY_KILL_2                  = -1542005,
    SAY_NOVA                    = -1542006,
    SAY_DIE                     = -1542007,

    SPELL_CORRUPTION            = 30938,
    SPELL_EVOCATION             = 30935,

    SPELL_FIRE_NOVA             = 33132,
    SPELL_FIRE_NOVA_H           = 37371,

    SPELL_SHADOW_BOLT_VOLLEY    = 28599,
    SPELL_SHADOW_BOLT_VOLLEY_H  = 40070,

    SPELL_BURNING_NOVA          = 30940,
    SPELL_VORTEX                = 37370,

    SPELL_CHANNELING            = 39123,
};

struct SortByAngle
{
    SortByAngle(WorldObject const* pRef): m_pRef(pRef) {}
    bool operator()(WorldObject* pLeft, WorldObject* pRight)
    {
        return m_pRef->GetAngle(pLeft) < m_pRef->GetAngle(pRight);
    }
    WorldObject const* m_pRef;
};

struct boss_kelidan_the_breakerAI : public ScriptedAI
{
    boss_kelidan_the_breakerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blood_furnace*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiSetupAddsTimer = 100;
        m_bDidMagtheridonYell = false;
        DoCastSpellIfCan(m_creature, SPELL_EVOCATION);
        Reset();
    }

    instance_blood_furnace* m_pInstance;

    bool m_bIsRegularMode;

    uint32 m_uiShadowVolleyTimer;
    uint32 m_uiBurningNovaTimer;
    uint32 m_uiFirenovaTimer;
    uint32 m_uiCorruptionTimer;
    uint32 m_uiSetupAddsTimer;
    uint8 m_uiKilledAdds;
    bool m_bDidMagtheridonYell;

    GuidVector m_vAddGuids;

    void Reset() override
    {
        m_uiShadowVolleyTimer   = 1000;
        m_uiBurningNovaTimer    = 15000;
        m_uiCorruptionTimer     = 5000;
        m_uiFirenovaTimer       = 0;
        m_uiKilledAdds          = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bDidMagtheridonYell && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->_IsWithinDist(pWho, 73.0f, false))
        {
            if (m_pInstance)
                m_pInstance->DoOrSimulateScriptTextForThisInstance(SAY_MAGTHERIDON_INTRO, NPC_MAGTHERIDON);

            m_bDidMagtheridonYell = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_WAKE, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DIE, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELIDAN_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELIDAN_EVENT, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_EVOCATION);
        m_uiSetupAddsTimer = 2000;
    }

    void DoSetupAdds()
    {
        m_uiSetupAddsTimer = 0;

        if (!m_pInstance)
            return;

        GuidList lAddGuids;
        m_pInstance->GetKelidanAddList(lAddGuids);

        // Sort Adds to vector if not already done
        if (!lAddGuids.empty())
        {
            m_vAddGuids.reserve(lAddGuids.size());
            std::list<Creature*> lAdds;
            for (GuidList::const_iterator itr = lAddGuids.begin(); itr != lAddGuids.end(); ++itr)
            {
                if (Creature* pAdd = m_pInstance->instance->GetCreature(*itr))
                    lAdds.push_back(pAdd);
            }
            // Sort them by angle
            lAdds.sort(SortByAngle(m_creature));
            for (std::list<Creature*>::const_iterator itr = lAdds.begin(); itr != lAdds.end(); ++itr)
                m_vAddGuids.push_back((*itr)->GetObjectGuid());
        }

        // Respawn killed adds and reset counter
        m_uiKilledAdds = 0;
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            Creature* pAdd = m_pInstance->instance->GetCreature(*itr);
            if (pAdd && !pAdd->isAlive())
                pAdd->Respawn();
        }

        // Cast pentagram
        uint8 s = m_vAddGuids.size();
        for (uint8 i = 0; i < s; ++i)
        {
            Creature* pCaster = m_pInstance->instance->GetCreature(m_vAddGuids[i]);
            Creature* pTarget = m_pInstance->instance->GetCreature(m_vAddGuids[(i + 2) % s]);
            if (pCaster && pTarget)
                pCaster->CastSpell(pTarget, SPELL_CHANNELING, false);
        }
    }

    void AddJustAggroed(Unit* pWho)
    {
        // Let all adds attack
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            Creature* pAdd = m_creature->GetMap()->GetCreature(*itr);
            if (pAdd && !pAdd->getVictim())
                pAdd->AI()->AttackStart(pWho);
        }
    }

    void AddJustReachedHome()
    {
        m_uiSetupAddsTimer = 2000;
    }

    void AddJustDied(Unit* pKiller)
    {
        ++m_uiKilledAdds;
        if (m_uiKilledAdds == MAX_ADDS)
        {
            m_creature->InterruptNonMeleeSpells(true);
            AttackStart(pKiller);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSetupAddsTimer)
        {
            if (m_uiSetupAddsTimer <= uiDiff)
                DoSetupAdds();
            else
                m_uiSetupAddsTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFirenovaTimer)
        {
            if (m_uiFirenovaTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FIRE_NOVA : SPELL_FIRE_NOVA_H) == CAST_OK)
                {
                    m_uiFirenovaTimer = 0;
                    m_uiShadowVolleyTimer = 2000;
                }
            }
            else
                m_uiFirenovaTimer -= uiDiff;
        }

        if (m_uiShadowVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT_VOLLEY : SPELL_SHADOW_BOLT_VOLLEY_H) == CAST_OK)
                m_uiShadowVolleyTimer = urand(5000, 13000);
        }
        else
            m_uiShadowVolleyTimer -= uiDiff;

        if (m_uiCorruptionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CORRUPTION) == CAST_OK)
                m_uiCorruptionTimer = urand(30000, 50000);
        }
        else
            m_uiCorruptionTimer -= uiDiff;

        if (m_uiBurningNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BURNING_NOVA, CAST_TRIGGERED) == CAST_OK)
            {
                DoScriptText(SAY_NOVA, m_creature);

                if (!m_bIsRegularMode)
                    DoCastSpellIfCan(m_creature, SPELL_VORTEX, CAST_TRIGGERED);

                m_uiBurningNovaTimer = urand(20000, 28000);
                m_uiFirenovaTimer = 5000;
            }
        }
        else
            m_uiBurningNovaTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelidan_the_breaker(Creature* pCreature)
{
    return new boss_kelidan_the_breakerAI(pCreature);
}

/*######
## mob_shadowmoon_channeler
######*/

enum
{
    SPELL_SHADOW_BOLT       = 12739,
    SPELL_SHADOW_BOLT_H     = 15472,

    SPELL_MARK_OF_SHADOW    = 30937,
};

struct mob_shadowmoon_channelerAI : public ScriptedAI
{
    mob_shadowmoon_channelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShadowBoltTimer;
    uint32 m_uiMarkOfShadowTimer;

    void Reset() override
    {
        m_uiShadowBoltTimer = urand(1000, 2000);
        m_uiMarkOfShadowTimer = urand(5000, 7000);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->InterruptNonMeleeSpells(false);

        switch (urand(0, 2))
        {
            case 0:
                DoScriptText(SAY_ADD_AGGRO_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_ADD_AGGRO_2, m_creature);
                break;
            case 2:
                DoScriptText(SAY_ADD_AGGRO_3, m_creature);
                break;
        }

        if (!m_pInstance)
            return;

        if (Creature* pKelidan = m_pInstance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustAggroed(pWho);
    }

    void JustDied(Unit* pKiller) override
    {
        if (!m_pInstance)
            return;

        if (Creature* pKelidan = m_pInstance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustDied(pKiller);
    }

    void JustReachedHome() override
    {
        if (!m_pInstance)
            return;

        if (Creature* pKelidan = m_pInstance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustReachedHome();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMarkOfShadowTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_SHADOW) == CAST_OK)
                    m_uiMarkOfShadowTimer = urand(15000, 20000);
            }
        }
        else
            m_uiMarkOfShadowTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                    m_uiShadowBoltTimer = urand(5000, 6000);
            }
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_channeler(Creature* pCreature)
{
    return new mob_shadowmoon_channelerAI(pCreature);
}

void AddSC_boss_kelidan_the_breaker()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_kelidan_the_breaker";
    pNewScript->GetAI = &GetAI_boss_kelidan_the_breaker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadowmoon_channeler";
    pNewScript->GetAI = &GetAI_mob_shadowmoon_channeler;
    pNewScript->RegisterSelf();
}
