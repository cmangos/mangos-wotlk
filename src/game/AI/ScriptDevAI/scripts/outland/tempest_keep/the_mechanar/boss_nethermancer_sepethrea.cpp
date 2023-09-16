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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 95
SDComment: May need some small adjustments
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "mechanar.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = 19959,
    SAY_SUMMON                      = 19965,
    SAY_DRAGONS_BREATH_1            = 19963,
    SAY_DRAGONS_BREATH_2            = 19964,
    SAY_SLAY1                       = 19961,
    SAY_SLAY2                       = 19962,
    SAY_DEATH                       = 19966,

    SPELL_SUMMON_RAGING_FLAMES      = 35275,
    SPELL_SUMMON_RAGING_FLAMES_H    = 39084,
    SPELL_FROST_ATTACK              = 45196, // serverside - triggers 45195
    SPELL_ARCANE_BLAST              = 35314,
    SPELL_DRAGONS_BREATH            = 35250,

    NPC_RAGING_FLAMES               = 20481,
};

struct boss_nethermancer_sepethreaAI : public CombatAI
{
    boss_nethermancer_sepethreaAI(Creature* creature) : CombatAI(creature, 0), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* /*unit*/, float x, float /*y*/, float /*z*/)->bool
        {
            return x < 266.0f;
        });
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        m_creature->SetInCombatWithZone();
        DoBroadcastText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_FROST_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_SUMMON_RAGING_FLAMES : SPELL_SUMMON_RAGING_FLAMES_H);

        if (m_instance)
            m_instance->SetData(TYPE_SEPETHREA, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SEPETHREA, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SEPETHREA, FAIL);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_ARCANE_BLAST:
                m_creature->getThreatManager().modifyThreatPercent(target, -50.0f);
                break;
            case SPELL_DRAGONS_BREATH:
                if (urand(0, 1))
                    DoBroadcastText(urand(0, 1) ? SAY_DRAGONS_BREATH_1 : SAY_DRAGONS_BREATH_2, m_creature);
                break;
        }
    }
};

enum
{
    SPELL_RAGING_FLAMES = 35281,
    SPELL_INFERNO       = 35268,
    SPELL_INFERNO_H     = 39346,
};

struct npc_raging_flamesAI : public CombatAI
{
    npc_raging_flamesAI(Creature* creature) : CombatAI(creature, 0), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        if (m_creature->IsTemporarySummon())
            m_summonerGuid = m_creature->GetSpawnerGuid();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_summonerGuid;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_RAGING_FLAMES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        FixateRandomTarget();
    }

    void FixateRandomTarget()
    {
        DoResetThreat();

        if (Creature* summoner = m_creature->GetMap()->GetCreature(m_summonerGuid))
            if (Unit* newTarget = summoner->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                m_creature->AddThreat(newTarget, 10000000.0f);
    }
};

void AddSC_boss_nethermancer_sepethrea()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nethermancer_sepethrea";
    pNewScript->GetAI = &GetNewAIInstance<boss_nethermancer_sepethreaAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_raging_flames";
    pNewScript->GetAI = &GetNewAIInstance<npc_raging_flamesAI>;
    pNewScript->RegisterSelf();
}
