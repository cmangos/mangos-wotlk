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
SDName: Boss_Patchwerk
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "Spells/SpellTargetDefines.h"
#include "naxxramas.h"

enum
{
    SAY_AGGRO1            = 13068,
    SAY_AGGRO2            = 13069,
    SAY_DEATH             = 13070,
    SAY_SLAY              = 13071,

    EMOTE_BERSERK         = 11694,
    EMOTE_ENRAGE          = 7798,

    SPELL_HATEFULSTRIKE_PRIMER  = 28307,
    SPELL_HATEFULSTRIKE         = 28308,
    SPELL_HATEFULSTRIKE_H       = 59192,
    SPELL_ENRAGE                = 28131,
    SPELL_BERSERK               = 26662,
    SPELL_SLIMEBOLT             = 32309,

    SPELLSET_NORMAL             = 1602801,
    SPELLSET_BERSERK            = 1602802,
};

enum PatchwerkActions
{
    PATCHWERK_ENRAGE_LOW,
    PATCHWERK_BERSERK,
    PATCHWERK_ACTION_MAX,
};

struct boss_patchwerkAI : public BossAI
{
    boss_patchwerkAI(Creature* creature) : BossAI(creature, PATCHWERK_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetDataType(TYPE_PATCHWERK);
        AddOnKillText(SAY_SLAY);
        AddOnDeathText(SAY_DEATH);
        AddOnAggroText(SAY_AGGRO1, SAY_AGGRO2);
        AddTimerlessCombatAction(PATCHWERK_ENRAGE_LOW, true);
        AddCombatAction(PATCHWERK_BERSERK, 6min);
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();
        m_creature->SetSpellList(SPELLSET_NORMAL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case PATCHWERK_ENRAGE_LOW:
                if (m_creature->GetHealthPercent() <= 5.f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    {
                        DoBroadcastText(EMOTE_ENRAGE, m_creature);
                        DisableCombatAction(action);
                    }
                }
                break;
            case PATCHWERK_BERSERK:
                DoBroadcastText(EMOTE_BERSERK, m_creature);
                m_creature->CastSpell(nullptr, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                m_creature->SetSpellList(SPELLSET_BERSERK);
                break;
        }
    }
};

struct HatefulStrikePrimer : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_HIGHEST_HP);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!spell->GetCaster()->CanReachWithMeleeAttack(target))
            return false;

        auto const& threatList = spell->GetCaster()->getThreatManager().getThreatList();
        uint32 i = 0;
        for (auto itr = threatList.begin(); itr != threatList.end() && i < (spell->GetCaster()->GetMap()->IsRegularDifficulty() ? 2 : 3); ++itr, ++i)
            if ((*itr)->getTarget() == target)
                return true;
        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            if (Unit* caster = spell->GetCaster())
            {
                // Target is filtered in Spell::FilterTargetMap
                if (Unit* unitTarget = spell->GetUnitTarget())
                    caster->CastSpell(unitTarget, SPELL_HATEFULSTRIKE, TRIGGERED_NONE);
            }
        }
    }
};

void AddSC_boss_patchwerk()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_patchwerk";
    pNewScript->GetAI = &GetNewAIInstance<boss_patchwerkAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<HatefulStrikePrimer>("spell_patchwerk_hatefulstrike");
}
