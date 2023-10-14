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
SDName: Boss_High_Botanist_Freywinn
SD%Complete: 90
SDComment: Timers may need some fine adjustments
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = 19944,
    SAY_KILL_1                  = 19945,
    SAY_KILL_2                  = 19946,
    SAY_TREE_1                  = 19947,
    SAY_TREE_2                  = 19948,
    SAY_DEATH                   = 19949,

    SPELL_TRANQUILITY           = 34550,
    SPELL_TREE_FORM             = 34551,
    SPELL_SUMMON_FRAYER         = 34557,
    SPELL_PLANT_WHITE           = 34759,
    SPELL_PLANT_GREEN           = 34761,
    SPELL_PLANT_BLUE            = 34762,
    SPELL_PLANT_RED             = 34763,

    SPELL_CRYPT_SCARAB_CONFUSE  = 16420,
    SPELL_PLANT_SPAWN_EFFECT    = 34770,
    SPELL_BIND_FEET_PERIODIC    = 34781,

    NPC_FRAYER_PROTECTOR        = 19953,

    SPELL_LIST_TREE_FORM        = 1797502,
    SPELL_LIST_TREE_FORM_H      = 2155802,
};

enum FreywinnActions
{
    FREYWINN_ACTION_MAX,
};

struct boss_high_botanist_freywinnAI : public CombatAI
{
    boss_high_botanist_freywinnAI(Creature* creature) : CombatAI(creature, FREYWINN_ACTION_MAX),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    uint8 m_uiFrayerAddsCount;
    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();
        m_creature->SetSpellList(m_creature->GetCreatureInfo()->SpellList);
        m_uiFrayerAddsCount = 0;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetInCombatWithZone();

        if (summoned->GetEntry() == NPC_FRAYER_PROTECTOR)
        {
            ++m_uiFrayerAddsCount;
            summoned->CastSpell(nullptr, SPELL_CRYPT_SCARAB_CONFUSE, TRIGGERED_NONE);
        }
        else
            summoned->CastSpell(nullptr, SPELL_PLANT_SPAWN_EFFECT, TRIGGERED_NONE);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_FRAYER_PROTECTOR)
        {
            --m_uiFrayerAddsCount;

            // When all 3 Frayers are killed stop the tree form action (if not done this already)
            if (!m_uiFrayerAddsCount && m_creature->HasAura(SPELL_TREE_FORM))
            {
                InterruptTreeForm();
            }
        }
    }

    void AddSeedlingCooldowns(uint32 spellId)
    {
        if (spellId != SPELL_PLANT_WHITE)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_PLANT_WHITE), nullptr, false, 5000);
        if (spellId != SPELL_PLANT_GREEN)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_PLANT_GREEN), nullptr, false, 5000);
        if (spellId != SPELL_PLANT_BLUE)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_PLANT_BLUE), nullptr, false, 5000);
        if (spellId != SPELL_PLANT_RED)
            m_creature->AddCooldown(*sSpellTemplate.LookupEntry<SpellEntry>(SPELL_PLANT_RED), nullptr, false, 5000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_PLANT_WHITE:
            case SPELL_PLANT_GREEN:
            case SPELL_PLANT_BLUE:
            case SPELL_PLANT_RED: AddSeedlingCooldowns(spellInfo->Id); break;
            case SPELL_TREE_FORM:
                DoBroadcastText(urand(0, 1) ? SAY_TREE_1 : SAY_TREE_2, m_creature);
                m_creature->CastSpell(nullptr, SPELL_SUMMON_FRAYER, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
                m_creature->SetSpellList(m_isRegularMode ? SPELL_LIST_TREE_FORM : SPELL_LIST_TREE_FORM_H);
                break;
        }
    }

    void InterruptTreeForm()
    {
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->RemoveAurasDueToSpell(SPELL_TREE_FORM);
        m_uiFrayerAddsCount = 0;
    }
};

void AddSC_boss_high_botanist_freywinn()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_high_botanist_freywinn";
    pNewScript->GetAI = &GetNewAIInstance<boss_high_botanist_freywinnAI>;
    pNewScript->RegisterSelf();
}
