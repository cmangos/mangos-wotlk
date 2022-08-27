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
SDName: Boss_Loatheb
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    EMOTE_AURA_BLOCKING     = 32334,
    EMOTE_AURA_WANE         = 32805,
    EMOTE_AURA_FADING       = 32335,

    SPELL_DEATHBLOOM        = 29865,
    SPELL_DEATHBLOOM_H      = 55053,
    SPELL_INEVITABLE_DOOM   = 29204,
    SPELL_INEVITABLE_DOOM_H = 55052,
    SPELL_NECROTIC_AURA     = 55593,
    SPELL_NECROTIC_PRE_WARN = 60929,
    SPELL_NECROTIC_WARN     = 59481,
    SPELL_SUMMON_SPORE      = 29234,
    SPELL_BERSERK           = 26662,

    NPC_SPORE               = 16286,
    NPC_LOATHEB             = 16011,

    SPELLSET_10N            = 1601101,
    SPELLSET_10N_P2         = 1601102,
    SPELLSET_25N            = 2971801,
    SPELLSET_25N_P2         = 2971802,
};

enum LoathebActions
{
    LOATHEB_BERSERK,
    LOATHEB_SOFT_ENRAGE,
    LOATHEB_ACTIONS_MAX,
};

struct boss_loathebAI : public BossAI
{
    boss_loathebAI(Creature* creature) : BossAI(creature, LOATHEB_ACTIONS_MAX),
        m_instance(static_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_LOATHEB);
        AddCombatAction(LOATHEB_SOFT_ENRAGE, 5min);
        if (!m_isRegularMode)
            AddCombatAction(LOATHEB_BERSERK, 12min);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        m_creature->SetSpellList(m_isRegularMode ? SPELLSET_10N : SPELLSET_25N);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_SPORE)
            return;

        summoned->AI()->AttackStart(m_creature);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SPORE && m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SPORE_LOSER, false);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LOATHEB_BERSERK:
            {
                DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                break;
            }
            case LOATHEB_SOFT_ENRAGE:
            {
                m_creature->SetSpellList(m_isRegularMode ? SPELLSET_10N_P2 : SPELLSET_25N_P2);
                DisableCombatAction(action);
                return;
            }
        }
        ResetCombatAction(action, 5min);
    }
};

struct NecroticAura : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (Unit* caster = spell->GetCaster())
            if (caster->GetEntry() == NPC_LOATHEB)
            {
                caster->CastSpell(nullptr, SPELL_NECROTIC_PRE_WARN, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
                caster->CastSpell(nullptr, SPELL_NECROTIC_WARN, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
            }
    }
};

struct AuraPreWarning : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (Unit* target = aura->GetTarget())
                DoBroadcastText(EMOTE_AURA_WANE, target);
    }
};

struct AuraWarning : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (Unit* target = aura->GetTarget())
                DoBroadcastText(EMOTE_AURA_FADING, target);
    }
};

void AddSC_boss_loatheb()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_loatheb";
    pNewScript->GetAI = &GetNewAIInstance<boss_loathebAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<NecroticAura>("spell_loatheb_necrotic_aura");
    RegisterSpellScript<AuraPreWarning>("spell_loatheb_prewarn");
    RegisterSpellScript<AuraWarning>("spell_loatheb_warn");
}
