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
#include "AI/ScriptDevAI/base/CombatAI.h"
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

    SPELL_SUMMON_HEATHEN            = 30737,
    SPELL_SUMMON_REAVER             = 30785,
    SPELL_SUMMON_SHARPSHOOTER       = 30786,

    TARGET_NUM                      = 8,

    NPC_SHATTERED_ASSASSIN          = 17695,
};

float AssassEntrance[3] = {275.136f, -84.29f, 2.3f};        // y -8
float AssassExit[3] = {184.233f, -84.29f, 2.3f};            // y -8
float AddsEntrance[3] = {306.036f, -84.29f, 1.93f};

enum WarchiefKargathActions
{
    WARCHIEF_KARGATH_BLADE_DANCE,
    WARCHIEF_KARGATH_CHARGE, // not a spell list event due to being tied to blade dance
    WARCHIEF_KARGATH_ACTIONS_MAX,
    WARCHIEF_KARGATH_COMBAT_SPAWN_ASSASSINS,
    WARCHIEF_KARGATH_COMBAT_SPAWN_ASSISTANT,
    WARCHIEF_KARGATH_COMBAT_BLADE_DANCE,
};

struct boss_warchief_kargath_bladefistAI : public CombatAI
{
    boss_warchief_kargath_bladefistAI(Creature* creature) : CombatAI(creature, WARCHIEF_KARGATH_ACTIONS_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float /*y*/, float /*z*/)
        {
            return x > 270.0f || x < 185.0f;
        });
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
        AddCombatAction(WARCHIEF_KARGATH_BLADE_DANCE, 45000u);
        AddCombatAction(WARCHIEF_KARGATH_CHARGE, true);
        AddCustomAction(WARCHIEF_KARGATH_COMBAT_SPAWN_ASSASSINS, 5000u, [&]() { SpawnAssassin(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(WARCHIEF_KARGATH_COMBAT_SPAWN_ASSISTANT, 20000u, [&]() { SpawnAssistant(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(WARCHIEF_KARGATH_COMBAT_BLADE_DANCE, true, [&]() { HandleBladeDance(); }, TIMER_COMBAT_COMBAT);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    GuidVector m_addGuids;
    GuidVector m_assassinGuids;

    GuidVector m_bladeDanceTargetGuids;

    uint32 m_summonedType;

    uint32 m_bladeDanceTargetCount;

    void Reset() override
    {
        CombatAI::Reset();
        m_summonedType = 0;
        m_bladeDanceTargetCount = 0;

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_BLADEFIST, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_HEARTHEN_GUARD:
            case NPC_SHARPSHOOTER_GUARD:
            case NPC_REAVER_GUARD:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    summoned->AI()->AttackStart(pTarget);

                m_addGuids.push_back(summoned->GetObjectGuid());
                break;
            case NPC_SHATTERED_ASSASSIN:
                m_assassinGuids.push_back(summoned->GetObjectGuid());
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        DoDespawnAdds();

        if (m_instance)
            m_instance->SetData(TYPE_BLADEFIST, DONE);
    }

    void JustReachedHome() override
    {
        DoDespawnAdds();

        if (m_instance)
            m_instance->SetData(TYPE_BLADEFIST, FAIL);
    }

    void OnLeash() override
    {
        DoBroadcastText(SAY_EVADE, m_creature);
    }

    // Note: this should be done by creature linkin in core
    void DoDespawnAdds()
    {
        for (GuidVector::const_iterator itr = m_addGuids.begin(); itr != m_addGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_addGuids.clear();

        for (GuidVector::const_iterator itr = m_assassinGuids.begin(); itr != m_assassinGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_assassinGuids.clear();
    }

    void SpawnAssassin()
    {
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] + 8, AssassEntrance[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] - 8, AssassEntrance[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] + 8, AssassExit[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] - 8, AssassExit[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 24000);
    }

    void SpawnAssistant()
    {
        Creature* warchiefsPortal = m_creature->GetMap()->GetCreature("SHH_WARCHIEFS_PORTAL");
        if (warchiefsPortal)
        {
            switch (m_summonedType)
            {
                case 0: warchiefsPortal->CastSpell(nullptr, SPELL_SUMMON_HEATHEN, TRIGGERED_OLD_TRIGGERED); break;
                case 1: warchiefsPortal->CastSpell(nullptr, SPELL_SUMMON_REAVER, TRIGGERED_OLD_TRIGGERED); break;
                case 2: warchiefsPortal->CastSpell(nullptr, SPELL_SUMMON_SHARPSHOOTER, TRIGGERED_OLD_TRIGGERED); break;
            }
        }

        m_summonedType = (m_summonedType + 1) % 3;
        ResetTimer(WARCHIEF_KARGATH_COMBAT_SPAWN_ASSISTANT, 20000);
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_BLADE_DANCE_TARGETING:
                m_bladeDanceTargetGuids.push_back(target->GetObjectGuid());
                break;
            case SPELL_BLADE_DANCE_CHARGE:
                ResetTimer(WARCHIEF_KARGATH_COMBAT_BLADE_DANCE, 500);
                m_creature->CastSpell(nullptr, SPELL_BLADE_DANCE, TRIGGERED_OLD_TRIGGERED);
                break;
        }            
    }

    void HandleBladeDance()
    {
        if (m_bladeDanceTargetCount == 0)
        {
            // stop bladedance
            SetCombatScriptStatus(false);
            SetCombatMovement(true, true);
            SetMeleeEnabled(true);
            if (!m_isRegularMode)
                ResetCombatAction(WARCHIEF_KARGATH_CHARGE, 500);
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
                --m_bladeDanceTargetCount;
            } while (result != SPELL_CAST_OK && m_bladeDanceTargetCount > 0);
            if (result != SPELL_CAST_OK)
            {
                ResetTimer(WARCHIEF_KARGATH_COMBAT_BLADE_DANCE, 1);
                m_bladeDanceTargetCount = 0;
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case WARCHIEF_KARGATH_BLADE_DANCE:
            {
                ResetTimer(WARCHIEF_KARGATH_COMBAT_BLADE_DANCE, 1);
                SetCombatScriptStatus(true);
                SetCombatMovement(false);
                SetMeleeEnabled(false);
                m_bladeDanceTargetGuids.clear();
                m_creature->CastSpell(nullptr, SPELL_BLADE_DANCE_TARGETING, TRIGGERED_NONE);
                m_bladeDanceTargetCount = std::min(m_bladeDanceTargetGuids.size(), size_t(TARGET_NUM));
                ResetCombatAction(action, 30000);
                break;
            }
            case WARCHIEF_KARGATH_CHARGE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(target, SPELL_CHARGE_H);
                break;
        }
    }
};

struct npc_blade_dance_targetAI : public ScriptedAI
{
    npc_blade_dance_targetAI(Creature* creature) : ScriptedAI(creature) {}
    void Reset() override {}
    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        damage = std::max(m_creature->GetMaxHealth(), damage);
    }
};

void AddSC_boss_warchief_kargath_bladefist()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warchief_kargath_bladefist";
    pNewScript->GetAI = &GetNewAIInstance<boss_warchief_kargath_bladefistAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blade_dance_target";
    pNewScript->GetAI = &GetNewAIInstance<npc_blade_dance_targetAI>;
    pNewScript->RegisterSelf();
}
