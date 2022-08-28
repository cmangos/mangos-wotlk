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
SDName: Boss_Temporus
SD%Complete: 90
SDComment:
Patch_2.2.0: Increased recast time for Temporus' Hasten spell on Heroic. GetSubsequentActionTimer(TEMPORUS_ACTION_HASTEN) -> 12-18 from 16-24
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "dark_portal.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO               = -1269001,
    SAY_BANISH              = -1269002,
    SAY_SLAY1               = 16825,
    SAY_SLAY2               = 16826,
    SAY_DEATH               = -1269005,

    SPELL_HASTEN            = 31458,
    SPELL_MORTAL_WOUND      = 31464,
    SPELL_WING_BUFFET       = 31475,
    SPELL_WING_BUFFET_H     = 38593,
    SPELL_REFLECTION        = 38592
};

enum TemporusActions // order based on priority
{
    TEMPORUS_ACTION_SPELL_REFLECTION,
    TEMPORUS_ACTION_WING_BUFFET,
    TEMPORUS_ACTION_HASTEN,
    TEMPORUS_ACTION_MORTAL_WOUND,
    TEMPORUS_ACTION_MAX
};

struct boss_temporusAI : public CombatAI
{
    boss_temporusAI(Creature* creature) : CombatAI(creature, TEMPORUS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        if (!m_isRegularMode)
            AddCombatAction(TEMPORUS_ACTION_SPELL_REFLECTION, GetInitialActionTimer(TEMPORUS_ACTION_SPELL_REFLECTION));
        AddCombatAction(TEMPORUS_ACTION_WING_BUFFET, GetInitialActionTimer(TEMPORUS_ACTION_WING_BUFFET));
        AddCombatAction(TEMPORUS_ACTION_HASTEN, GetInitialActionTimer(TEMPORUS_ACTION_HASTEN));
        AddCombatAction(TEMPORUS_ACTION_MORTAL_WOUND, GetInitialActionTimer(TEMPORUS_ACTION_MORTAL_WOUND));
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 GetInitialActionTimer(TemporusActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case TEMPORUS_ACTION_WING_BUFFET: return urand(18000, 22000);
                case TEMPORUS_ACTION_HASTEN: return urand(13000, 18000);
                case TEMPORUS_ACTION_MORTAL_WOUND: return urand(3500, 7000);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case TEMPORUS_ACTION_SPELL_REFLECTION: return urand(17000, 22000);
                case TEMPORUS_ACTION_WING_BUFFET: return urand(14000, 18000);
                case TEMPORUS_ACTION_HASTEN: return urand(10000, 15000);
                case TEMPORUS_ACTION_MORTAL_WOUND: return urand(3500, 7000);
                default: return 0;
            }
        }
    }

    uint32 GetSubsequentActionTimer(TemporusActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case TEMPORUS_ACTION_WING_BUFFET: return urand(20000, 28000);
                case TEMPORUS_ACTION_HASTEN: return urand(17000, 21000);
                case TEMPORUS_ACTION_MORTAL_WOUND: return urand(4500, 7000);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case TEMPORUS_ACTION_SPELL_REFLECTION: return urand(25000, 35000);
                case TEMPORUS_ACTION_WING_BUFFET: return urand(14000, 28000);
                case TEMPORUS_ACTION_HASTEN: return urand(12000, 21000);
                case TEMPORUS_ACTION_MORTAL_WOUND: return urand(4500, 7000);
                default: return 0;
            }
        }
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) // spells should only reset their action timer on success
    {
        switch (spellInfo->Id)
        {
			case SPELL_REFLECTION:
				ResetCombatAction(TEMPORUS_ACTION_SPELL_REFLECTION, GetSubsequentActionTimer(TemporusActions(TEMPORUS_ACTION_SPELL_REFLECTION)));
				break;
			case SPELL_WING_BUFFET:
			case SPELL_WING_BUFFET_H:
				ResetCombatAction(TEMPORUS_ACTION_WING_BUFFET, GetSubsequentActionTimer(TemporusActions(TEMPORUS_ACTION_WING_BUFFET)));
				break;
			case SPELL_HASTEN:
				ResetCombatAction(TEMPORUS_ACTION_HASTEN, GetSubsequentActionTimer(TemporusActions(TEMPORUS_ACTION_HASTEN)));
				break;
			case SPELL_MORTAL_WOUND:
				ResetCombatAction(TEMPORUS_ACTION_MORTAL_WOUND, GetSubsequentActionTimer(TemporusActions(TEMPORUS_ACTION_MORTAL_WOUND)));
				break;
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        // Despawn Time Keeper
        if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_TIME_KEEPER)
        {
            if (m_creature->IsWithinDistInMap(who, 20.0f))
            {
                if (DoCastSpellIfCan(who, SPELL_BANISH_HELPER) == CAST_OK)
                    DoScriptText(SAY_BANISH, m_creature);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TEMPORUS_ACTION_SPELL_REFLECTION:
                DoCastSpellIfCan(m_creature, SPELL_REFLECTION);
                break;
            case TEMPORUS_ACTION_WING_BUFFET:
                DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_WING_BUFFET : SPELL_WING_BUFFET_H);
                break;
            case TEMPORUS_ACTION_HASTEN:
                DoCastSpellIfCan(m_creature, SPELL_HASTEN);
                break;
            case TEMPORUS_ACTION_MORTAL_WOUND:
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MORTAL_WOUND);
                break;
        }
    }
};

void AddSC_boss_temporus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_temporus";
    pNewScript->GetAI = &GetNewAIInstance<boss_temporusAI>;
    pNewScript->RegisterSelf();
}
