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
SDName: Boss_Murmur
SD%Complete: 75
SDComment: Sonic Boom and Murmur's Touch require additional research and core support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "shadow_labyrinth.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    EMOTE_SONIC_BOOM            = 18799,

    // Intro spells - used on npcs
    SPELL_SUPPRESSION_BLAST     = 33332,
    SPELL_MURMURS_WRATH         = 33331,
    SPELL_MURMURS_WRATH_2       = 33329,

    SPELL_MAGNETIC_PULL         = 33689,
    SPELL_SONIC_BOOM            = 33923,        // dummy spell - triggers 33666
    SPELL_SONIC_BOOM_H          = 38796,        // dummy spell - triggers 38795
    SPELL_MURMURS_TOUCH         = 33711,        // on expire silences the party members using shockwave - 33686 - also related to spell 33760
    SPELL_MURMURS_TOUCH_H       = 38794,
    SPELL_RESONANCE             = 33657,

    SPELL_SONIC_SHOCK           = 38797,        // Heroic Spell
    SPELL_THUNDERING_STORM      = 39365,        // Heroic Spell
};

enum MurmurActions
{
    MURMUR_ACTION_MAX,
    MURMUR_OOC_RP_ATTACK,
    MURMUR_OOC_CASTER_ATTACK
};

struct boss_murmurAI : public CombatAI
{
    boss_murmurAI(Creature* creature) : CombatAI(creature, MURMUR_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCustomAction(MURMUR_OOC_RP_ATTACK, true, [&]() { HandleOocAttack(); }, TIMER_COMBAT_OOC);
        AddCustomAction(MURMUR_OOC_CASTER_ATTACK, true, [&]() { HandleOocCasterAttack(); }, TIMER_COMBAT_OOC);
    }

    ScriptedInstance* m_instance;
    bool m_bIsRegularMode;

    GuidVector spellbindersVector;
    GuidVector summonersVector;

    uint32 m_uiResonanceTimer;
    uint32 m_uiThunderingStormTimer;

    void Reset() override
    {
        CombatAI::Reset();

        m_creature->SetHealthPercent(40.f);
        SetAIImmobilizedState(true);
    }


    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            ResetTimer(MURMUR_OOC_CASTER_ATTACK, urand(8000, 10000));
            ResetTimer(MURMUR_OOC_RP_ATTACK, urand(8000, 10000));
            m_instance->GetCreatureGuidVectorFromStorage(NPC_CABAL_SPELLBINDER, spellbindersVector);
            m_instance->GetCreatureGuidVectorFromStorage(NPC_CABAL_SUMMONER, summonersVector);
        }
    }

    void HandleOocAttack()
    {
        if (m_creature->IsInCombat())
            return;

        // kill one that's moving
        if (urand(0, 1))
        {
            GuidVector moversVector;
            for (ObjectGuid& guid : spellbindersVector)
            {
                if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                {
                    if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                    {
                        moversVector.push_back(guid);
                    }
                }
            }
            for (ObjectGuid& guid : summonersVector)
            {
                if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                {
                    if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                    {
                        moversVector.push_back(guid);
                    }
                }
            }
            if (moversVector.size() > 0)
            {
                if (ObjectGuid& guid = moversVector[urand(0, moversVector.size() - 1)])
                {
                    if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
                    {
                        DoCastSpellIfCan(creature, SPELL_MURMURS_WRATH);
                    }
                }
            }
        }
        // stun 5 targets
        else
            DoCastSpellIfCan(nullptr, SPELL_SUPPRESSION_BLAST);

        ResetTimer(MURMUR_OOC_RP_ATTACK, 3000);
    }

    void HandleOocCasterAttack()
    {
        if (m_creature->IsInCombat())
            return;

        for (ObjectGuid& guid : spellbindersVector)
        {
            if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
            {
                m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, creature);
            }
        }
        for (ObjectGuid& guid : summonersVector)
        {
            if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
            {
                m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, creature);
            }
        }

        ResetTimer(MURMUR_OOC_CASTER_ATTACK, urand(3000, 8000));
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_SONIC_BOOM || spellInfo->Id == SPELL_SONIC_BOOM_H)
            DoBroadcastText(EMOTE_SONIC_BOOM, m_creature);
    }
};

// 33332 - Suppression Blast
struct SuppressionBlast : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(5);
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsInCombat())
            return false;

        return true;
    }
};

// 39365 - Thundering Storm
struct ThunderingStorm : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetCaster();
        float dist = sqrt(target->GetDistance(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), DIST_CALC_NONE));
        if (dist < 25.f || dist > 100.f)
            return false;
        return true;
    }
};

void AddSC_boss_murmur()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_murmur";
    pNewScript->GetAI = &GetNewAIInstance<boss_murmurAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SuppressionBlast>("spell_suppression_blast");
    RegisterSpellScript<ThunderingStorm>("spell_thundering_storm");
}
