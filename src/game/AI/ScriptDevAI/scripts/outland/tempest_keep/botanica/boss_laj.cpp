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
SDName: Boss_Laj
SD%Complete: 90
SDComment: Immunities are wrong, must be adjusted to use resistance from creature_templates. Most spells require database support.
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    EMOTE_SUMMON                = 10755,

    SPELL_ALLERGIC_REACTION     = 34697,
    SPELL_TELEPORT_SELF         = 34673,
    SPELL_TRASH                 = 3391,

    SPELL_SUMMON_LASHER_1       = 34681,
    SPELL_SUMMON_FLAYER_1       = 34682,
    SPELL_SUMMON_LASHER_2       = 34684,
    SPELL_SUMMON_FLAYER_2       = 34685,
    // 3 and 4 not confirmed to be used
    SPELL_SUMMON_LASHER_3       = 34686,
    SPELL_SUMMON_FLAYER_4       = 34687,
    SPELL_SUMMON_LASHER_4       = 34688,
    SPELL_SUMMON_FLAYER_3       = 34690,

    SPELL_ROOT_SELF             = 23973,

    SPELL_LAJ_ARCANE            = 34703,
    SPELL_LAJ_FIRE              = 34704,
    SPELL_LAJ_FROST             = 34705,
    SPELL_LAJ_NATURE            = 34707,
    SPELL_LAJ_SHADOW            = 34710,

    MODEL_ID_DEFAULT            = 13109,
    MODEL_ID_ARCANE             = 14213,
    MODEL_ID_FIRE               = 13110,
    MODEL_ID_FROST              = 14112,
    MODEL_ID_NATURE             = 14214,
};

const std::vector<uint32> summonSpellsFirstLoc = { SPELL_SUMMON_LASHER_1, SPELL_SUMMON_FLAYER_1 };
const std::vector<uint32> summonSpellsSecondLoc = { SPELL_SUMMON_LASHER_2, SPELL_SUMMON_FLAYER_2 };

enum LajActions
{
    LAJ_ACTION_MAX,
    LAJ_TELEPORT_SUMMON
};

struct boss_lajAI : public CombatAI
{
    boss_lajAI(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(LAJ_TELEPORT_SUMMON, true, [&]() { HandleTeleportSummon(); }, TIMER_COMBAT_COMBAT);
    }

    GuidVector m_spawns;

    void Reset() override
    {
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        DespawnGuids(m_spawns);
        CombatAI::Reset();
    }

    void EnterEvadeMode() override
    {
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        DespawnGuids(m_spawns);
        CombatAI::EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DespawnGuids(m_spawns);
    }

    void AddTransformCooldowns(uint32 spellId)
    {
        if (spellId != SPELL_LAJ_ARCANE)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LAJ_ARCANE), nullptr, false, 5000);
        if (spellId != SPELL_LAJ_FIRE)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LAJ_FIRE), nullptr, false, 5000);
        if (spellId != SPELL_LAJ_FROST)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LAJ_FROST), nullptr, false, 5000);
        if (spellId != SPELL_LAJ_NATURE)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LAJ_NATURE), nullptr, false, 5000);
        if (spellId != SPELL_LAJ_SHADOW)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LAJ_SHADOW), nullptr, false, 5000);
    }

    void DoTransform()
    {
        uint32 spellId = 0;
        // Random transform into a different form
        switch (urand(0, 4))
        {
            case 0: spellId = SPELL_LAJ_ARCANE; break;
            case 1: spellId = SPELL_LAJ_FIRE; break;
            case 2: spellId = SPELL_LAJ_FROST; break;
            case 3: spellId = SPELL_LAJ_NATURE; break;
            case 4: spellId = SPELL_LAJ_SHADOW; break;
        }
        DoCastSpellIfCan(nullptr, spellId);
    }

    void DoSummons()
    {
        DoCastSpellIfCan(nullptr, summonSpellsFirstLoc[urand(0, summonSpellsFirstLoc.size() - 1)], CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, summonSpellsSecondLoc[urand(0, summonSpellsSecondLoc.size() - 1)], CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_ROOT_SELF, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            summoned->AI()->AttackStart(target);

        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_LAJ_ARCANE:
            case SPELL_LAJ_FIRE:
            case SPELL_LAJ_FROST:
            case SPELL_LAJ_NATURE:
            case SPELL_LAJ_SHADOW: AddTransformCooldowns(spellInfo->Id); break;
            case SPELL_TELEPORT_SELF: 
                // Remove CombatMovement
                SetCombatMovement(false, true); 
                // Remove the target focus 
                SetCombatScriptStatus(true);
                SetMeleeEnabled(false);
                m_creature->SetTarget(nullptr);
                ResetTimer(LAJ_TELEPORT_SUMMON, 4000); 
                break;
        }
    }

    void HandleTeleportSummon()
    {
        DoSummons();
        DoBroadcastText(EMOTE_SUMMON, m_creature);

        SetCombatScriptStatus(false);

        SetMeleeEnabled(true);
        SetCombatMovement(true, true);
    }
};

void AddSC_boss_laj()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_laj";
    pNewScript->GetAI = &GetNewAIInstance<boss_lajAI>;
    pNewScript->RegisterSelf();
}
