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
SDName: Boss_Faerlina
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "naxxramas.h"

enum
{
    SAY_GREET                   = 12852,
    SAY_DEATH                   = 12853,
    SAY_SLAY_1                  = 12854,
    SAY_SLAY_2                  = 12855,
    SAY_AGGRO_1                 = 12856,
    SAY_AGGRO_2                 = 12857,
    SAY_AGGRO_3                 = 12858,
    SAY_AGGRO_4                 = 12859,

    EMOTE_BOSS_GENERIC_FRENZY   = 1191,
    EMOTE_WIDOWS_EMBRACE        = 31019,

    SPELL_POISONBOLT_VOLLEY     = 28796,
    SPELL_POISONBOLT_VOLLEY_H   = 54098,
    SPELL_ENRAGE                = 28798,
    SPELL_ENRAGE_H              = 54100,
    SPELL_RAIN_OF_FIRE          = 28794,
    SPELL_RAIN_OF_FIRE_H        = 54099,
    SPELL_WIDOWS_EMBRACE        = 28732,
    SPELL_WIDOWS_EMBRACE_H      = 54097,
    SPELL_WIDOWS_EMBRACE_CD     = 28797,

    FAERLINA_GRACE_TIMER        = 0,

    SPELLSET_10N                = 1595301,
    SPELLSET_25N                = 2926801,
};

struct boss_faerlinaAI : public BossAI
{
    boss_faerlinaAI(Creature* creature) : BossAI(creature, 0),
        m_instance(static_cast<instance_naxxramas *>(creature->GetInstanceData())),
        m_hasTaunted(false)
    {
        SetDataType(TYPE_FAERLINA);
        AddOnAggroText(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3, SAY_AGGRO_4);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddOnDeathText(SAY_DEATH);
    }

    instance_naxxramas* m_instance;
    bool m_hasTaunted;

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasTaunted && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 80.0f) &&  m_creature->IsWithinLOSInMap(who))
        {
            DoBroadcastText(SAY_GREET, m_creature);
            m_hasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // Check if we hit with Widow's Embrave
        if (spellInfo->Id == SPELL_WIDOWS_EMBRACE || spellInfo->Id == SPELL_WIDOWS_EMBRACE_H)
        {
            // Achievement 'Momma said Knock you out': If we removed OR delayed the frenzy, the criteria is failed
            if (TimeSinceEncounterStart() >= 30s)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_YOU_OUT, false);
        }
    }
};

struct WidowsEmbrace : public AuraScript, public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->Suicide();
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            return;
        Unit* target = aura->GetTarget();
        DoBroadcastText(EMOTE_WIDOWS_EMBRACE, target);
        bool isRegularDifficulty = target->GetMap()->IsRegularDifficulty();
        target->RemoveAurasDueToSpell(isRegularDifficulty ? SPELL_ENRAGE : SPELL_ENRAGE_H);
        target->CastSpell(nullptr, SPELL_WIDOWS_EMBRACE_CD, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_faerlina()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_faerlina";
    pNewScript->GetAI = &GetNewAIInstance<boss_faerlinaAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<WidowsEmbrace>("spell_widows_embrace");
}
