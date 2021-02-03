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
SDName: boss_zarithrian
SD%Complete: 100
SDComment:
SDCategory: Ruby Sanctum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ruby_sanctum.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO           = -1724019,
    SAY_SLAY_1          = -1724020,
    SAY_SLAY_2          = -1724021,
    SAY_DEATH           = -1724022,
    SAY_SUMMON          = -1724023,

    SPELL_SUMMON_FLAMECALLER    = 74398,
    SPELL_CLEAVE_ARMOR          = 74367,
    SPELL_INTIMIDATING_ROAR     = 74384,

    NPC_ONYX_FLAMECALLER        = 39814,                // handled in ACID
};

enum ZarithrianActions
{
    ZARITHRIAN_CLEAVE_ARMOR,
    ZARITHRIAN_INTIMIDATING_ROAR,
    ZARITHRIAN_SUMMON_FLAMECALLER,
    ZARITHRIAN_ACTION_MAX,
};

struct boss_zarithrianAI : public CombatAI
{
    boss_zarithrianAI(Creature* creature) : CombatAI(creature, ZARITHRIAN_ACTION_MAX), m_instance(static_cast<instance_ruby_sanctum*>(creature->GetInstanceData()))
    {
        AddCombatAction(ZARITHRIAN_CLEAVE_ARMOR, 8000u);
        AddCombatAction(ZARITHRIAN_INTIMIDATING_ROAR, 14000u);
        AddCombatAction(ZARITHRIAN_SUMMON_FLAMECALLER, 15000u);
    }

    instance_ruby_sanctum* m_instance;

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ZARITHRIAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ZARITHRIAN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ZARITHRIAN, FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ZARITHRIAN_CLEAVE_ARMOR:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE_ARMOR) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case ZARITHRIAN_INTIMIDATING_ROAR:
                if (DoCastSpellIfCan(m_creature, SPELL_INTIMIDATING_ROAR) == CAST_OK)
                    ResetCombatAction(action, 32000);
                break;
            case ZARITHRIAN_SUMMON_FLAMECALLER:
                if (m_instance)
                {
                    GuidList m_lStalkersGuidList;
                    m_instance->GetSpawnStalkersGuidList(m_lStalkersGuidList);

                    // each stalker summons a flamekaller; there are 2 on 10 player mode and 4 on 25 player mode
                    for (const auto& guid : m_lStalkersGuidList)
                        if (Creature* pStalker = m_creature->GetMap()->GetCreature(guid))
                            pStalker->CastSpell(pStalker, SPELL_SUMMON_FLAMECALLER, TRIGGERED_OLD_TRIGGERED);

                    DoScriptText(SAY_SUMMON, m_creature);
                    ResetCombatAction(action, 45000);
                }
                break;
        }
    }
};

void AddSC_boss_zarithrian()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_zarithrian";
    pNewScript->GetAI = &GetNewAIInstance<boss_zarithrianAI>;
    pNewScript->RegisterSelf();
}
