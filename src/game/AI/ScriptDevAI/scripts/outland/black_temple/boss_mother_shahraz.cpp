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
SDName: Boss_Mother_Shahraz
SD%Complete: 99
SDComment: TODO: Implement prenerf beams
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // Speech'n'Sounds
    SAY_ATTRACTION_1            = -1564018,
    SAY_ATTRACTION_2            = -1564019,
    SAY_ATTRACTION_3            = -1564020,
    SAY_AGGRO                   = -1564021,
    SAY_BEAM_1                  = -1564022,
    SAY_BEAM_2                  = -1564023,
    SAY_BEAM_3                  = -1564024,
    SAY_SLAY_1                  = -1564025,
    SAY_SLAY_2                  = -1564026,
    SAY_ENRAGE                  = -1564027,
    SAY_DEATH                   = -1564028,

    // Spells
    SPELL_SINFUL_PERIODIC       = 40862,        // periodic triggers 40827
    SPELL_SINISTER_PERIODIC     = 40863,        // periodic triggers 40859
    SPELL_VILE_PERIODIC         = 40865,        // periodic triggers 40860
    SPELL_WICKED_PERIODIC       = 40866,        // periodic triggers 40861
    SPELL_RANDOM_PERIODIC       = 40867,
    SPELL_FATAL_ATTRACTION      = 40869,        // dummy, triggers 41001
    SPELL_SILENCING_SHRIEK      = 40823,
    SPELL_SABER_LASH_PROC       = 40816,        // procs 40810 and 43690 on melee damage
    SPELL_FRENZY                = 23537,
    SPELL_BERSERK               = 45078,
};

static const uint32 aPrismaticAuras[] =
{
    40880,                                                  // Shadow
    40882,                                                  // Fire
    40883,                                                  // Nature
    40891,                                                  // Arcane
    40896,                                                  // Frost
    40897,                                                  // Holy
};

static const uint32 aPeriodicBeams[] = {SPELL_SINFUL_PERIODIC, SPELL_SINISTER_PERIODIC, SPELL_VILE_PERIODIC, SPELL_WICKED_PERIODIC};

enum ShahrazActions
{
    SHAHRAZ_ACTION_BERSERK,
    SHAHRAZ_ACTION_FRENZY,
    SHAHRAZ_ACTION_FATAL_ATTRACTION,
    SHAHRAZ_ACTION_BEAM,
    SHAHRAZ_ACTION_SHRIEK,
    SHAHRAZ_ACTION_PRISMATIC_SHIELD,
    SHAHRAZ_ACTION_MAX,
};

struct boss_shahrazAI : public CombatAI
{
    boss_shahrazAI(Creature* creature) : CombatAI(creature, SHAHRAZ_ACTION_MAX), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData()))
    {
        AddCombatAction(SHAHRAZ_ACTION_BERSERK, GetInitialActionTimer(SHAHRAZ_ACTION_BERSERK));
        AddTimerlessCombatAction(SHAHRAZ_ACTION_FRENZY, true);
        AddCombatAction(SHAHRAZ_ACTION_FATAL_ATTRACTION, GetInitialActionTimer(SHAHRAZ_ACTION_FATAL_ATTRACTION));
        AddCombatAction(SHAHRAZ_ACTION_BEAM, GetInitialActionTimer(SHAHRAZ_ACTION_BEAM));
        AddCombatAction(SHAHRAZ_ACTION_SHRIEK, GetInitialActionTimer(SHAHRAZ_ACTION_SHRIEK));
        AddCombatAction(SHAHRAZ_ACTION_PRISMATIC_SHIELD, GetInitialActionTimer(SHAHRAZ_ACTION_PRISMATIC_SHIELD));
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    instance_black_temple* m_instance;

    uint8 m_currentBeam;

    void Reset() override
    {
        CombatAI::Reset();

        m_currentBeam               = urand(0, 3);

        m_creature->RemoveAurasDueToSpell(SPELL_RANDOM_PERIODIC);
        DoCastSpellIfCan(nullptr, SPELL_SABER_LASH_PROC, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    uint32 GetInitialActionTimer(ShahrazActions id)
    {
        switch (id)
        {
            case SHAHRAZ_ACTION_BERSERK: return 10 * MINUTE * IN_MILLISECONDS;
            case SHAHRAZ_ACTION_FATAL_ATTRACTION: return urand(22000, 29000);
            case SHAHRAZ_ACTION_BEAM: return 15000;
            case SHAHRAZ_ACTION_SHRIEK: return 30000;
            case SHAHRAZ_ACTION_PRISMATIC_SHIELD: return 15000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(ShahrazActions id)
    {
        switch (id)
        {
            case SHAHRAZ_ACTION_FATAL_ATTRACTION: return urand(22000, 40000);
            case SHAHRAZ_ACTION_BEAM: return 20000;
            case SHAHRAZ_ACTION_SHRIEK: return 30000;
            case SHAHRAZ_ACTION_PRISMATIC_SHIELD: return 15000;
            default: return 0;
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHAHRAZ, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHAHRAZ, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHAHRAZ, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHAHRAZ_ACTION_FRENZY:
            {
                if (m_creature->GetHealthPercent() < 10.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_RANDOM_PERIODIC) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        DisableCombatAction(SHAHRAZ_ACTION_BEAM);
                        SetActionReadyStatus(action, false); // timerless
                    }
                }
                return;
            }
            case SHAHRAZ_ACTION_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_ENRAGE, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case SHAHRAZ_ACTION_BEAM:
            {
                m_currentBeam = (m_currentBeam + urand(1, 3)) % 4;
                if (DoCastSpellIfCan(nullptr, aPeriodicBeams[m_currentBeam]) == CAST_OK)
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_BEAM_1, m_creature); break;
                        case 1: DoScriptText(SAY_BEAM_2, m_creature); break;
                        case 2: DoScriptText(SAY_BEAM_3, m_creature); break;
                    }
                    ResetCombatAction(action, GetSubsequentActionTimer(ShahrazActions(action)));
                }
                return;
            }
            case SHAHRAZ_ACTION_PRISMATIC_SHIELD:
            {
                if (DoCastSpellIfCan(nullptr, aPrismaticAuras[urand(0, 5)]) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(ShahrazActions(action)));
                return;
            }
            case SHAHRAZ_ACTION_FATAL_ATTRACTION:
            {
                if (m_creature->getThreatManager().getThreatList().size() < 3)
                    return;
                if (DoCastSpellIfCan(nullptr, SPELL_FATAL_ATTRACTION) == CAST_OK)
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_ATTRACTION_1, m_creature); break;
                        case 1: DoScriptText(SAY_ATTRACTION_3, m_creature); break;
                        case 2: DoScriptText(SAY_ATTRACTION_3, m_creature); break;
                    }
                    ResetCombatAction(action, GetSubsequentActionTimer(ShahrazActions(action)));
                }
                return;
            }
            case SHAHRAZ_ACTION_SHRIEK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SILENCING_SHRIEK) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(ShahrazActions(action)));
                return;
            }
        }
    }
};

UnitAI* GetAI_boss_shahraz(Creature* creature)
{
    return new boss_shahrazAI(creature);
}

void AddSC_boss_mother_shahraz()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_mother_shahraz";
    pNewScript->GetAI = &GetAI_boss_shahraz;
    pNewScript->RegisterSelf();
}
