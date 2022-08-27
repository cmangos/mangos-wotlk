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

#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
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

    FAERLINA_GRACE_TIMER        = 0,

    SPELLSET_10N                = 1595301,
    SPELLSET_25N                = 2926801,
};

struct boss_faerlinaAI : public BossAI
{
    boss_faerlinaAI(Creature* creature) : BossAI(creature, 0),
    m_instance(static_cast<instance_naxxramas *>(creature->GetInstanceData())),
    m_hasTaunted(false),
    m_inGracePeriod(false)
    {
        SetDataType(TYPE_FAERLINA);
        AddOnAggroText(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3, SAY_AGGRO_4);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddOnDeathText(SAY_DEATH);
    }

    instance_naxxramas* m_instance;
    bool m_hasTaunted;
    bool m_inGracePeriod;

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);
        m_inGracePeriod = true;
        AddCustomAction(FAERLINA_GRACE_TIMER, 30s, [&](){
            m_inGracePeriod = false;
        });
    }

    void Reset() override
    {
        BossAI::Reset();
        m_creature->SetSpellList(m_creature->GetMap()->IsRegularDifficulty() ? SPELLSET_10N : SPELLSET_25N);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_hasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 80.0f) &&  m_creature->IsWithinLOSInMap(pWho))
        {
            DoBroadcastText(SAY_GREET, m_creature);
            m_hasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpellEntry) override
    {
        // Check if we hit with Widow's Embrave
        if (pSpellEntry->Id == SPELL_WIDOWS_EMBRACE || pSpellEntry->Id == SPELL_WIDOWS_EMBRACE_H)
        {
            // Achievement 'Momma said Knock you out': If we removed OR delayed the frenzy, the criteria is failed
            if ((!m_inGracePeriod) && m_instance)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_YOU_OUT, false);
        }
    }
};

struct WidowsEmbrace : public AuraScript, public SpellScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (Creature* caster = dynamic_cast<Creature*>(aura->GetCaster()))
            if (caster->IsAlive() && caster->GetEntry() == NPC_FAERLINA_FOLLOWER)
                caster->Suicide();
        if (!apply)
            return;
        if (Creature* target = dynamic_cast<Creature*>(aura->GetTarget()))
        {
            DoBroadcastText(EMOTE_WIDOWS_EMBRACE, target);
            bool isRegularDifficulty = target->GetMap()->IsRegularDifficulty();
            target->AddCooldown(*(aura->GetSpellProto()));
            target->RemoveAurasDueToSpell(isRegularDifficulty ? SPELL_ENRAGE : SPELL_ENRAGE_H);
        }
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
