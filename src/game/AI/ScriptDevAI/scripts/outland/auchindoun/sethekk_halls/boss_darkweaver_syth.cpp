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
SDName: Boss_Darkweaver_Syth
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_SUMMON                  = -1556000,
    SAY_AGGRO_1                 = -1556001,
    SAY_AGGRO_2                 = -1556002,
    SAY_AGGRO_3                 = -1556003,
    SAY_SLAY_1                  = -1556004,
    SAY_SLAY_2                  = -1556005,
    SAY_DEATH                   = -1556006,

    SPELL_FROST_SHOCK           = 12548,
    SPELL_FROST_SHOCK_H         = 21401,
    SPELL_FLAME_SHOCK           = 15039,
    SPELL_FLAME_SHOCK_H         = 15616,
    SPELL_SHADOW_SHOCK          = 33620,
    SPELL_SHADOW_SHOCK_H        = 38136,
    SPELL_ARCANE_SHOCK          = 33534,
    SPELL_ARCANE_SHOCK_H        = 38135,

    SPELL_CHAIN_LIGHTNING       = 15659,
    SPELL_CHAIN_LIGHTNING_H     = 15305,

    SPELL_SUMMON_SYTH_FIRE      = 33537,                    // Spawns 19203
    SPELL_SUMMON_SYTH_ARCANE    = 33538,                    // Spawns 19205
    SPELL_SUMMON_SYTH_FROST     = 33539,                    // Spawns 19204
    SPELL_SUMMON_SYTH_SHADOW    = 33540,                    // Spawns 19206

    // Npc entries
    NPC_FIRE_ELEMENTAL          = 19203,
    NPC_FROST_ELEMENTAL         = 19204,
    NPC_ARCANE_ELEMENTAL        = 19205,
    NPC_SHADOW_ELEMENTAL        = 19206,
};

enum SythActions
{
    SYTH_90,
    SYTH_55,
    SYTH_15,
    SYTH_ACTION_MAX
};

struct boss_darkweaver_sythAI : public CombatAI
{
    boss_darkweaver_sythAI(Creature* creature) : CombatAI(creature, SYTH_ACTION_MAX), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(SYTH_90, true);
        AddTimerlessCombatAction(SYTH_55, true);
        AddTimerlessCombatAction(SYTH_15, true);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    bool m_isRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }
    }
    
    void JustReachedHome() override
    {
        m_creature->RemoveGuardians();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        m_creature->RemoveGuardians();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            summoned->AI()->AttackStart(Target);
    }

    // Wrapper to handle the elementals summon
    void SythSummoning()
    {
        DoBroadcastText(SAY_SUMMON, m_creature);

        m_creature->CastSpell(nullptr, SPELL_SUMMON_SYTH_ARCANE, TRIGGERED_IGNORE_COOLDOWNS); // front
        m_creature->CastSpell(nullptr, SPELL_SUMMON_SYTH_FIRE, TRIGGERED_IGNORE_COOLDOWNS); // back
        m_creature->CastSpell(nullptr, SPELL_SUMMON_SYTH_FROST, TRIGGERED_IGNORE_COOLDOWNS); // left
        m_creature->CastSpell(nullptr, SPELL_SUMMON_SYTH_SHADOW, TRIGGERED_IGNORE_COOLDOWNS); // right
    }

    void ExecuteAction(uint32 action) override
    {
        if (!CanExecuteCombatAction())
            return;

        switch (action)
        {
            case SYTH_90:
                if (m_creature->GetHealthPercent() < 90.0f)
                {
                    SythSummoning();
                    SetActionReadyStatus(action, false);
                }
                break;
            case SYTH_55:
                if (m_creature->GetHealthPercent() < 55.0f)
                {
                    SythSummoning();
                    SetActionReadyStatus(action, false);
                }
                break;
            case SYTH_15:
                if (m_creature->GetHealthPercent() < 15.0f)
                {
                    SythSummoning();
                    SetActionReadyStatus(action, false);
                }
                break;
        }
    }
};

void AddSC_boss_darkweaver_syth()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_darkweaver_syth";
    pNewScript->GetAI = &GetNewAIInstance<boss_darkweaver_sythAI>;
    pNewScript->RegisterSelf();
}
