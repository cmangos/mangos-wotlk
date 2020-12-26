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
    SAY_ENTER = -1534067,
    SAY_KILL1 = -1534068,
    SAY_KILL2 = -1534069,
    SAY_KILL3 = -1534070,
    SAY_DOOM1 = -1534071,
    SAY_DOOM2 = -1534072,
    SAY_DEATH = -1534073,

    // abilities
    SPELL_CLEAVE            = 31345,
    SPELL_RAIN_OF_FIRE      = 31340,
    SPELL_HOWL_OF_AZGALOR   = 31344,
    SPELL_DOOM              = 31347,
    SPELL_ENRAGE            = 26662,
};

enum AzgalorActions
{
    AZGALOR_ACTION_CLEAVE,
    AZGALOR_RAIN_OF_FIRE,
    AZGALOR_HOWL_OF_AZGALOR,
    AZGALOR_DOOM,
    AZGALOR_ENRAGE,
    AZGALOR_ACTION_MAX,
};

static const float SPELL_RAIN_OF_FIRE_MINRANGE = 0;             // 0 yards from boss
static const float SPELL_RAIN_OF_FIRE_MAXRANGE = 34;            // 34 yards from boss

struct boss_azgalorAI : public CombatAI
{
    boss_azgalorAI(Creature* creature) : CombatAI(creature, AZGALOR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(AZGALOR_ACTION_CLEAVE, GetInitialActionTimer(AZGALOR_ACTION_CLEAVE));
        AddCombatAction(AZGALOR_RAIN_OF_FIRE, GetInitialActionTimer(AZGALOR_RAIN_OF_FIRE));
        AddCombatAction(AZGALOR_HOWL_OF_AZGALOR, GetInitialActionTimer(AZGALOR_HOWL_OF_AZGALOR));
        AddCombatAction(AZGALOR_DOOM, GetInitialActionTimer(AZGALOR_DOOM));
        AddCombatAction(AZGALOR_ENRAGE, GetInitialActionTimer(AZGALOR_ENRAGE));
        m_rainOfFireParams.range.minRange = SPELL_RAIN_OF_FIRE_MINRANGE;
        m_rainOfFireParams.range.maxRange = SPELL_RAIN_OF_FIRE_MAXRANGE;
        AddOnKillText(SAY_KILL1, SAY_KILL2, SAY_KILL3);
        Reset();
    }

    ScriptedInstance* m_instance;

    SelectAttackingTargetParams m_rainOfFireParams;

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case AZGALOR_ACTION_CLEAVE: return urand(10000, 16000);
            case AZGALOR_RAIN_OF_FIRE: return 25000;
            case AZGALOR_HOWL_OF_AZGALOR: return 30000;
            case AZGALOR_DOOM: return urand(45000, 55000);
            case AZGALOR_ENRAGE: return 600000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case AZGALOR_ACTION_CLEAVE: return urand(8000, 16000);
            case AZGALOR_RAIN_OF_FIRE: return 15000;
            case AZGALOR_HOWL_OF_AZGALOR: return urand(18000, 20000);
            case AZGALOR_DOOM: return urand(45000, 55000);
            case AZGALOR_ENRAGE: return 300000;
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
        DoScriptText(SAY_DEATH, m_creature);
        if (m_instance)
            m_instance->SetData(TYPE_AZGALOR, DONE);
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DOOM)
            DoScriptText(urand(0, 1) ? SAY_DOOM1 : SAY_DOOM2, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case AZGALOR_ACTION_CLEAVE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case AZGALOR_RAIN_OF_FIRE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RAIN_OF_FIRE, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_rainOfFireParams))
                    if (DoCastSpellIfCan(target, SPELL_RAIN_OF_FIRE) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case AZGALOR_HOWL_OF_AZGALOR:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_HOWL_OF_AZGALOR) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case AZGALOR_DOOM:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DOOM) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case AZGALOR_ENRAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
        }
    }
};

struct AzgalorDoom : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->GetVictim() == target)
            return false;
        return true;
    }
};

void AddSC_boss_azgalor()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_azgalor";
    pNewScript->GetAI = &GetNewAIInstance<boss_azgalorAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<AzgalorDoom>("spell_azgalor_doom");
}
