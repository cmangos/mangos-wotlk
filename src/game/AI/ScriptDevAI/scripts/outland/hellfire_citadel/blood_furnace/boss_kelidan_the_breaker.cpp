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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "blood_furnace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    MAX_ADDS                    = 5,

    SAY_WAKE                    = -1542000,
    SAY_ADD_AGGRO_1             = -1542001,
    SAY_ADD_AGGRO_2             = -1542002,
    SAY_ADD_AGGRO_3             = -1542003,
    SAY_KILL_1                  = -1542004,
    SAY_KILL_2                  = -1542005,
    SAY_NOVA                    = -1542006,
    SAY_DIE                     = -1542007,

    SPELL_CORRUPTION_SD         = 30938,
    SPELL_EVOCATION             = 30935,

    SPELL_FIRE_NOVA             = 33775,
    SPELL_FIRE_NOVA_H           = 37371,

    SPELL_SHADOW_BOLT_VOLLEY    = 17228,
    SPELL_SHADOW_BOLT_VOLLEY_H  = 40070,

    SPELL_BURNING_NOVA          = 30940,
    SPELL_VORTEX                = 37370,

    SPELL_CHANNELING            = 39123,
};

enum KelidanActions
{
    KELIDAN_FIRE_NOVA,
    KELIDAN_BURNING_NOVA,
    KELIDAN_SHADOW_BOLT_VOLLEY,
    KELIDAN_CORRUPTION,
    KELIDAN_ACTION_MAX,
    KELIDAN_SETUP_ADDS,
};

struct boss_kelidan_the_breakerAI : public CombatAI
{
    boss_kelidan_the_breakerAI(Creature* creature) : CombatAI(creature, KELIDAN_ACTION_MAX), m_instance(static_cast<instance_blood_furnace*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_bDidMagtheridonYell(false)
    {
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float, float y, float)
        {
            return y < -158.23f;
        });
        AddTimerlessCombatAction(KELIDAN_FIRE_NOVA, false);
        AddCombatAction(KELIDAN_BURNING_NOVA, 15000u);
        AddCombatAction(KELIDAN_SHADOW_BOLT_VOLLEY, 1000u);
        AddCombatAction(KELIDAN_CORRUPTION, 5000u);
        AddCustomAction(KELIDAN_SETUP_ADDS, 100u, [&]() { DoSetupAdds(); });
    }

    instance_blood_furnace* m_instance;

    bool m_isRegularMode;

    uint8 m_uiKilledAdds;
    bool m_bDidMagtheridonYell;

    GuidVector m_vAddGuids;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiKilledAdds          = 0;
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(nullptr, SPELL_EVOCATION);
        CombatAI::JustRespawned();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_WAKE, m_creature);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DIE, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_KELIDAN_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KELIDAN_EVENT, FAIL);

        ResetTimer(KELIDAN_SETUP_ADDS, 2000);
    }

    void DoSetupAdds()
    {
        DoCastSpellIfCan(nullptr, SPELL_EVOCATION);
        if (!m_instance)
            return;

        GuidList lAddGuids;
        m_instance->GetKelidanAddList(lAddGuids);

        // Sort Adds to vector if not already done
        if (!lAddGuids.empty())
        {
            m_vAddGuids.reserve(lAddGuids.size());
            CreatureList lAdds;
            for (GuidList::const_iterator itr = lAddGuids.begin(); itr != lAddGuids.end(); ++itr)
            {
                if (Creature* pAdd = m_instance->instance->GetCreature(*itr))
                    lAdds.push_back(pAdd);
            }
            // Sort them by angle
            lAdds.sort([=](Creature* left, Creature* right) -> bool {return m_creature->GetAngle(left) < m_creature->GetAngle(right); });
            for (std::list<Creature*>::const_iterator itr = lAdds.begin(); itr != lAdds.end(); ++itr)
                m_vAddGuids.push_back((*itr)->GetObjectGuid());
        }

        // Respawn killed adds and reset counter
        m_uiKilledAdds = 0;
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            Creature* pAdd = m_instance->instance->GetCreature(*itr);
            if (pAdd)
            {
                if (pAdd->GetCombatManager().IsEvadingHome())
                {
                    ResetTimer(KELIDAN_SETUP_ADDS, 3000);
                    return;
                }
                if (!pAdd->IsAlive())
                    pAdd->Respawn();
            }
        }

        // Cast pentagram
        uint8 s = m_vAddGuids.size();
        for (uint8 i = 0; i < s; ++i)
        {
            Creature* caster = m_instance->instance->GetCreature(m_vAddGuids[i]);
            Creature* target = m_instance->instance->GetCreature(m_vAddGuids[(i + 2) % s]);
            if (caster && target)
                caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, caster);
        }

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void AddJustAggroed(Unit* who)
    {
        // Let all adds attack
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            Creature* pAdd = m_creature->GetMap()->GetCreature(*itr);
            if (pAdd && !pAdd->GetVictim())
                pAdd->AI()->AttackStart(who);
        }
    }

    void AddJustReachedHome()
    {
        ResetTimer(KELIDAN_SETUP_ADDS, 2000);
    }

    void AddJustDied(Unit* killer)
    {
        ++m_uiKilledAdds;
        if (m_uiKilledAdds == MAX_ADDS)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            m_creature->InterruptNonMeleeSpells(true);
            AttackStart(killer);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KELIDAN_FIRE_NOVA:
                if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_FIRE_NOVA : SPELL_FIRE_NOVA_H) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            case KELIDAN_BURNING_NOVA:
                if (DoCastSpellIfCan(nullptr, SPELL_BURNING_NOVA, CAST_TRIGGERED) == CAST_OK)
                {
                    DoScriptText(SAY_NOVA, m_creature);

                    if (!m_isRegularMode)
                        DoCastSpellIfCan(nullptr, SPELL_VORTEX, CAST_TRIGGERED);

                    SetActionReadyStatus(KELIDAN_FIRE_NOVA, true);
                    ResetCombatAction(action, urand(20000, 28000));
                }
                break;
            case KELIDAN_SHADOW_BOLT_VOLLEY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_SHADOW_BOLT_VOLLEY : SPELL_SHADOW_BOLT_VOLLEY_H) == CAST_OK)
                    ResetCombatAction(action, urand(5000, 13000));
                break;
            case KELIDAN_CORRUPTION:
                if (DoCastSpellIfCan(nullptr, SPELL_CORRUPTION_SD) == CAST_OK)
                    ResetCombatAction(action, urand(30000, 50000));
                break;
        }
    }
};

/*######
## mob_shadowmoon_channeler
######*/

enum
{
    SPELL_SHADOW_BOLT       = 12739,
    SPELL_SHADOW_BOLT_H     = 15472,

    SPELL_MARK_OF_SHADOW    = 30937,
};

enum ChannelerActions
{
    CHANNELER_MARK_OF_SHADOW,
    CHANNELER_SHADOW_BOLT,
    CHANNELER_ACTION_MAX,
    CHANNELER_SETUP_TIMER,
};

struct mob_shadowmoon_channelerAI : public CombatAI
{
    mob_shadowmoon_channelerAI(Creature* creature) : CombatAI(creature, CHANNELER_ACTION_MAX), m_instance(static_cast<instance_blood_furnace*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float, float y, float)
            {
                return y < -158.23f;
            });
        AddCombatAction(CHANNELER_MARK_OF_SHADOW, 5000, 7000);
        AddCombatAction(CHANNELER_SHADOW_BOLT, 1000, 2000);
        AddCustomAction(CHANNELER_SETUP_TIMER, true, [&]() { HandleSetup(); });
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_target;

    void Aggro(Unit* who) override
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

        if (!m_instance)
            return;

        if (Creature* pKelidan = m_instance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustAggroed(who);
    }

    void JustDied(Unit* killer) override
    {
        if (!m_instance)
            return;

        if (Creature* pKelidan = m_instance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustDied(killer);
    }

    void JustReachedHome() override
    {
        if (!m_instance)
            return;

        if (Creature* pKelidan = m_instance->GetSingleCreatureFromStorage(NPC_KELIDAN_THE_BREAKER))
            if (boss_kelidan_the_breakerAI* pKelidanAI = dynamic_cast<boss_kelidan_the_breakerAI*>(pKelidan->AI()))
                pKelidanAI->AddJustReachedHome();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            ResetTimer(CHANNELER_SETUP_TIMER, 2000);
            m_target = invoker->GetObjectGuid();
        }
    }

    void HandleSetup()
    {
        if (m_creature->GetCombatManager().IsInEvadeMode())
        {
            ResetTimer(CHANNELER_SETUP_TIMER, 1000);
            return;
        }

        if (Creature* target = m_creature->GetMap()->GetCreature(m_target))
            m_creature->CastSpell(target, SPELL_CHANNELING, TRIGGERED_NONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case CHANNELER_MARK_OF_SHADOW:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_MARK_OF_SHADOW) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 20000));
                break;
            case CHANNELER_SHADOW_BOLT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                        ResetCombatAction(action, urand(5000, 6000));
                break;
        }
    }
};

void AddSC_boss_kelidan_the_breaker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kelidan_the_breaker";
    pNewScript->GetAI = &GetNewAIInstance<boss_kelidan_the_breakerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadowmoon_channeler";
    pNewScript->GetAI = &GetNewAIInstance<mob_shadowmoon_channelerAI>;
    pNewScript->RegisterSelf();
}
