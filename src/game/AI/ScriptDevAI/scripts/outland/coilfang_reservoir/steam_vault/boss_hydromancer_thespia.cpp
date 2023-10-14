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
SDName: Boss_Hydromancer_Thespia
SD%Complete: 80
SDComment: Timers may need small adjustments; Elementals summon needs further research
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_hydromancer_thespia
mob_coilfang_waterelemental
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "steam_vault.h"

enum
{
    SAY_SUMMON                  = -1545000, // has no bct, but has sound - keeping it as script text
    SAY_CLOUD                   = 19456,
    SAY_AGGRO_1                 = 17696,
    SAY_AGGRO_2                 = 17697,
    SAY_AGGRO_3                 = 17698,
    SAY_SLAY_1                  = 17701,
    SAY_SLAY_2                  = 17702,
    SAY_DEAD                    = 17704,

    SPELL_LIGHTNING_CLOUD       = 25033,
    SPELL_LUNG_BURST            = 31481,
    SPELL_ENVELOPING_WINDS      = 31718,
    SPELL_SUMMON_ELEMENTALS     = 31476,            // not sure where to use this
};

struct boss_thespiaAI : public CombatAI
{
    boss_thespiaAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    ScriptedInstance* m_instance;
    bool m_bIsRegularMode;

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HYDROMANCER_THESPIA, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEAD, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HYDROMANCER_THESPIA, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_HYDROMANCER_THESPIA, IN_PROGRESS);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_LIGHTNING_CLOUD)
            if (urand(0, 1))
                DoBroadcastText(SAY_CLOUD, m_creature);
    }
};

void AddSC_boss_hydromancer_thespia()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hydromancer_thespia";
    pNewScript->GetAI = &GetNewAIInstance<boss_thespiaAI>;
    pNewScript->RegisterSelf();
}
