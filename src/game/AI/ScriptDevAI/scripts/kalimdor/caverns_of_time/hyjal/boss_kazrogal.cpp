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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjal.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_ENTER = -1534060,
    SAY_MARK1 = -1534061,
    SAY_MARK2 = -1534062,
    SAY_KILL1 = -1534063,
    SAY_KILL2 = -1534064,
    SAY_KILL3 = -1534065,
    SOUND_DEATH = 11018,

    // abilities
    SPELL_MALEVOLENT_CLEAVE = 31436,
    SPELL_WAR_STOMP         = 31480,
    SPELL_CRIPPLE           = 31477,
    SPELL_MARK_OF_KAZROGAL  = 31447,
};

enum KazrogalActions
{
    KAZROGAL_ACTION_MALEVOLENT_CLEAVE,
    KAZROGAL_ACTION_WAR_STOMP,
    KAZROGAL_ACTION_MARK_OF_KAZROGAL,
    KAZROGAL_ACTION_CRIPPLE,
    KAZROGAL_ACTION_MAX,
};

struct boss_kazrogalAI : public CombatAI
{
    boss_kazrogalAI(Creature* creature) : CombatAI(creature, KAZROGAL_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(KAZROGAL_ACTION_MALEVOLENT_CLEAVE, GetInitialActionTimer(KAZROGAL_ACTION_MALEVOLENT_CLEAVE));
        AddCombatAction(KAZROGAL_ACTION_WAR_STOMP, GetInitialActionTimer(KAZROGAL_ACTION_WAR_STOMP));
        AddCombatAction(KAZROGAL_ACTION_CRIPPLE, GetInitialActionTimer(KAZROGAL_ACTION_CRIPPLE));
        AddCombatAction(KAZROGAL_ACTION_MARK_OF_KAZROGAL, GetInitialActionTimer(KAZROGAL_ACTION_MARK_OF_KAZROGAL));
        AddOnKillText(SAY_KILL1, SAY_KILL2, SAY_KILL3);
        Reset();
    }

    ScriptedInstance* m_instance;
    uint32 m_markOfKazrogalCounter;

    void Reset() override
    {
        CombatAI::Reset();

        m_markOfKazrogalCounter = 0;
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case KAZROGAL_ACTION_MALEVOLENT_CLEAVE: return urand(6000, 21000);
            case KAZROGAL_ACTION_WAR_STOMP: return urand(12000, 18000);
            case KAZROGAL_ACTION_CRIPPLE: return 15000;
            case KAZROGAL_ACTION_MARK_OF_KAZROGAL: return 45000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case KAZROGAL_ACTION_MALEVOLENT_CLEAVE: return urand(6000, 21000);
            case KAZROGAL_ACTION_WAR_STOMP: return urand(15000, 30000);
            case KAZROGAL_ACTION_CRIPPLE: return urand(12000, 20000);
            case KAZROGAL_ACTION_MARK_OF_KAZROGAL: 
                return 45000 - std::min(m_markOfKazrogalCounter, uint32(7)) * 5000; // reduce each use by 5000 until 10000
            default: return 0; // never occurs but for compiler
        }
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoScriptText(SAY_ENTER, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->PlayDirectSound(SOUND_DEATH);
        if (m_instance)
            m_instance->SetData(TYPE_KAZROGAL, DONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KAZROGAL_ACTION_MALEVOLENT_CLEAVE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MALEVOLENT_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case KAZROGAL_ACTION_WAR_STOMP:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_WAR_STOMP) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case KAZROGAL_ACTION_CRIPPLE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CRIPPLE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CRIPPLE) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case KAZROGAL_ACTION_MARK_OF_KAZROGAL:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_MARK_OF_KAZROGAL) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_MARK1 : SAY_MARK2, m_creature);
                    ++m_markOfKazrogalCounter;
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                }
                break;
            }
        }
    }
};

struct MarkOfKazrogal : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetPowerType() != POWER_MANA)
            return false;

        return true;
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (aura->GetTarget()->IsPlayer() && target->GetPower(target->GetPowerType()) == 0)
        {
            target->CastSpell(nullptr, 31463, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
            target->RemoveAurasDueToSpell(aura->GetId());
        }
    }
};

void AddSC_boss_kazrogal()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kazrogal";
    pNewScript->GetAI = &GetNewAIInstance<boss_kazrogalAI>;
    pNewScript->RegisterSelf();

    RegisterScript<MarkOfKazrogal>("spell_mark_of_kazrogal");
}