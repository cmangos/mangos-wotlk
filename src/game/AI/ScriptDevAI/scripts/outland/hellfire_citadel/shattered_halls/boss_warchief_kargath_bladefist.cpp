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
SDName: Boss_Warchief_Kargath_Bladefist
SD%Complete: 90
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_warchief_kargath_bladefist
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shattered_halls.h"

enum
{
    SAY_AGGRO1                      = -1540042,
    SAY_AGGRO2                      = -1540043,
    SAY_AGGRO3                      = -1540044,
    SAY_SLAY1                       = -1540045,
    SAY_SLAY2                       = -1540046,
    SAY_DEATH                       = -1540047,
    SAY_EVADE                       = -1540048,

    SPELL_BLADE_DANCE_CHARGE        = 30751,
    SPELL_BLADE_DANCE_TARGETING     = 30738,
    SPELL_BLADE_DANCE               = 30739,
    SPELL_CHARGE_H                  = 25821,
    SPELL_DOUBLE_ATTACK             = 19818,

    TARGET_NUM                      = 8,

    NPC_SHATTERED_ASSASSIN          = 17695,
    NPC_HEARTHEN_GUARD              = 17621,
    NPC_SHARPSHOOTER_GUARD          = 17622,
    NPC_REAVER_GUARD                = 17623,
};

float AssassEntrance[3] = {275.136f, -84.29f, 2.3f};        // y -8
float AssassExit[3] = {184.233f, -84.29f, 2.3f};            // y -8
float AddsEntrance[3] = {306.036f, -84.29f, 1.93f};

struct boss_warchief_kargath_bladefistAI : public ScriptedAI
{
    boss_warchief_kargath_bladefistAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float y, float z)
        {
            return x > 270.0f || x < 185.0f;
        });        
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    GuidVector m_vAddGuids;
    GuidVector m_vAssassinGuids;

    GuidVector m_bladeDanceTargetGuids;

    uint32 m_uiChargeTimer;
    uint32 m_uiBladeDanceTimer;
    uint32 m_uiSummonAssistantTimer;
    uint32 m_uiWaitTimer;

    uint32 m_uiAssassinsTimer;

    uint32 m_uiSummoned;
    bool m_bInBlade;

    uint32 m_uiTargetNum;

    void Reset() override
    {
        m_uiSummoned = 0;
        m_bInBlade = false;
        m_uiWaitTimer = 0;

        m_uiChargeTimer = 0;
        m_uiBladeDanceTimer = 45000;
        m_uiSummonAssistantTimer = 20000;
        m_uiAssassinsTimer = 5000;

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);

        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_HEARTHEN_GUARD:
            case NPC_SHARPSHOOTER_GUARD:
            case NPC_REAVER_GUARD:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);

                m_vAddGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_SHATTERED_ASSASSIN:
                m_vAssassinGuids.push_back(pSummoned->GetObjectGuid());
                break;
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoDespawnAdds();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, DONE);
    }

    void JustReachedHome() override
    {
        DoDespawnAdds();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, FAIL);
    }

    void OnLeash() override
    {
        DoScriptText(SAY_EVADE, m_creature);
    }

    // Note: this should be done by creature linkin in core
    void DoDespawnAdds()
    {
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_vAddGuids.clear();

        for (GuidVector::const_iterator itr = m_vAssassinGuids.begin(); itr != m_vAssassinGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_vAssassinGuids.clear();
    }

    void SpawnAssassin()
    {
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] + 8, AssassEntrance[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] - 8, AssassEntrance[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] + 8, AssassExit[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] - 8, AssassExit[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_BLADE_DANCE_TARGETING:
                m_bladeDanceTargetGuids.push_back(target->GetObjectGuid());
                break;
            case SPELL_BLADE_DANCE_CHARGE:
                m_uiWaitTimer = 500;
                m_creature->CastSpell(nullptr, SPELL_BLADE_DANCE, TRIGGERED_OLD_TRIGGERED);
                break;
        }            
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget())
        {
            if (GetCombatScriptStatus())
                EnterEvadeMode();
            return;
        }

        if (m_uiAssassinsTimer)
        {
            if (m_uiAssassinsTimer <= uiDiff)
            {
                SpawnAssassin();
                m_uiAssassinsTimer = 0;
            }
            else
                m_uiAssassinsTimer -= uiDiff;
        }

        if (m_bInBlade)
        {
            if (m_uiWaitTimer)
            {
                if (m_uiWaitTimer <= uiDiff)
                {
                    if (m_uiTargetNum == 0)
                    {
                        // stop bladedance
                        m_bInBlade = false;
                        SetCombatScriptStatus(false);
                        SetCombatMovement(true, true);
                        SetMeleeEnabled(true);
                        m_uiWaitTimer = 0;
                        if (!m_bIsRegularMode)
                            m_uiChargeTimer = 500;
                    }
                    else
                    {
                        // move in bladedance
                        SpellCastResult result = SPELL_NOT_FOUND;
                        do
                        {
                            uint32 index = urand(0, m_bladeDanceTargetGuids.size() - 1);
                            if (Unit* target = m_creature->GetMap()->GetCreature(m_bladeDanceTargetGuids[index]))
                                // Until neutral target type TARGET_UNIT stops rolling for hit, need to force it
                                // should not send SMSG_SPELL_START but triggered spell cast bypasses checks which are necessary
                                result = m_creature->CastSpell(target, SPELL_BLADE_DANCE_CHARGE, TRIGGERED_IGNORE_HIT_CALCULATION);
                            m_bladeDanceTargetGuids.erase(m_bladeDanceTargetGuids.begin() + index);
                            --m_uiTargetNum;
                        } while (result != SPELL_CAST_OK && m_uiTargetNum > 0);
                        if (result != SPELL_CAST_OK)
                        {
                            m_uiWaitTimer = 1;
                            m_uiTargetNum = 0;
                        }
                        else
                            m_uiWaitTimer = 0;
                    }
                }
                else
                    m_uiWaitTimer -= uiDiff;
            }
        }
        else                                                // !m_bInBlade
        {
            if (m_uiBladeDanceTimer < uiDiff)
            {
                m_uiWaitTimer = 1;
                m_bInBlade = true;
                SetCombatScriptStatus(true);
                SetCombatMovement(false);
                SetMeleeEnabled(false);
                m_uiBladeDanceTimer = 30000;
                m_bladeDanceTargetGuids.clear();
                m_creature->CastSpell(nullptr, SPELL_BLADE_DANCE_TARGETING, TRIGGERED_NONE);
                m_uiTargetNum = std::min(m_bladeDanceTargetGuids.size(), size_t(TARGET_NUM));
                return;
            }
            m_uiBladeDanceTimer -= uiDiff;

            if (m_uiChargeTimer)
            {
                if (m_uiChargeTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        DoCastSpellIfCan(pTarget, SPELL_CHARGE_H);

                    m_uiChargeTimer = 0;
                }
                else
                    m_uiChargeTimer -= uiDiff;
            }

            if (m_uiSummonAssistantTimer < uiDiff)
            {
                switch (m_uiSummoned)
                {
                    case 0: m_creature->SummonCreature(NPC_HEARTHEN_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 20000); break;
                    case 1: m_creature->SummonCreature(NPC_REAVER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 20000); break;
                    case 2: m_creature->SummonCreature(NPC_SHARPSHOOTER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 20000); break;
                }
                
                m_uiSummoned++;
                
                if (m_uiSummoned == 3)
                    m_uiSummoned = 0;

                m_uiSummonAssistantTimer = 20000;
            }
            else
                m_uiSummonAssistantTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

UnitAI* GetAI_boss_warchief_kargath_bladefist(Creature* pCreature)
{
    return new boss_warchief_kargath_bladefistAI(pCreature);
}

struct npc_blade_dance_targetAI : public ScriptedAI
{
    npc_blade_dance_targetAI(Creature* creature) : ScriptedAI(creature) {}
    void Reset() override {}
    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        damage = std::max(m_creature->GetMaxHealth(), damage);
    }
};

UnitAI* GetAI_npc_blade_dance_target(Creature* pCreature)
{
    return new npc_blade_dance_targetAI(pCreature);
}

void AddSC_boss_warchief_kargath_bladefist()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warchief_kargath_bladefist";
    pNewScript->GetAI = &GetAI_boss_warchief_kargath_bladefist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blade_dance_target";
    pNewScript->GetAI = &GetAI_npc_blade_dance_target;
    pNewScript->RegisterSelf();
}
