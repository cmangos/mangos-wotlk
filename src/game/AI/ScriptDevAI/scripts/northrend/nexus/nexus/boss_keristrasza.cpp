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
SDName: Boss_Keristrasza
SD%Complete: 100%
SDComment:
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                   = 26176,
    SAY_CRYSTAL_NOVA            = 29618,
    SAY_ENRAGE                  = 29619,
    SAY_KILL                    = 29620,
    SAY_DEATH                   = 29621,

    MAX_INTENSE_COLD_STACK      = 2,            // the max allowed stacks for the achiev to pass

    SPELL_INTENSE_COLD          = 48094,
    SPELL_INTENSE_COLD_AURA     = 48095,        // used for Intense cold achiev

    SPELL_CRYSTALFIRE_BREATH    = 48096,
    SPELL_CRYSTALFIRE_BREATH_H  = 57091,

    SPELL_CRYSTALLIZE           = 48179,        // heroic
    SPELL_CRYSTAL_CHAINS        = 50997,        // normal

    SPELL_TAIL_SWEEP            = 50155,

    SPELL_ENRAGE                = 8599
};

enum KeristraszaActions
{
    KERISTRASZA_ACTION_MAX,
};

/*######
## boss_keristrasza
######*/

struct boss_keristraszaAI : public BossAI
{
    boss_keristraszaAI(Creature* creature) : BossAI(creature, KERISTRASZA_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnAggroText(SAY_AGGRO);
        AddOnDeathText(SAY_DEATH);
        AddOnKillText(SAY_KILL);
        SetDataType(TYPE_KERISTRASZA);
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        BossAI::Reset();

        if (!m_instance)
            return;

        if (m_creature->IsAlive())
        {
            if (m_instance->GetData(TYPE_KERISTRASZA) != SPECIAL)
                DoCastSpellIfCan(nullptr, SPELL_FROZEN_PRISON, CAST_TRIGGERED);
        }
    }

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);

        DoCastSpellIfCan(nullptr, SPELL_INTENSE_COLD, CAST_AURA_NOT_PRESENT);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_CRYSTALLIZE)
            DoBroadcastText(SAY_CRYSTAL_NOVA, m_creature);
        else if (spellInfo->Id == SPELL_ENRAGE)
            DoBroadcastText(SAY_ENRAGE, m_creature);
    }
};

// 48095 - Intense Cold
struct IntenseColdAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        // check only for heroic
        if (target->GetMap()->IsRegularDifficulty())
            return;

        // check achiev
        if (aura->GetStackAmount() > MAX_INTENSE_COLD_STACK)
        {
            if (instance_nexus* pInstance = static_cast<instance_nexus*>(target->GetInstanceData()))
                pInstance->SetData(TYPE_INTENSE_COLD_FAILED, target->GetGUIDLow());
        }
    }
};

void AddSC_boss_keristrasza()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_keristrasza";
    pNewScript->GetAI = &GetNewAIInstance<boss_keristraszaAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<IntenseColdAura>("spell_intense_cold_aura");
}
