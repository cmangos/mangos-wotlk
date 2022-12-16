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
SDName: Boss_Moorabi
SD%Complete: 20%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1604011,
    SAY_QUAKE                   = -1604012,
    SAY_TRANSFORM               = -1604013,
    SAY_SLAY_1                  = -1604014,
    SAY_SLAY_2                  = -1604015,
    SAY_SLAY_3                  = -1604016,
    SAY_DEATH                   = -1604017,
    EMOTE_TRANSFORM             = -1604018,
    EMOTE_TRANSFORMED           = -1604029,

    // Troll form
    SPELL_DETERMINED_STAB   = 55104,
    SPELL_MOJO_FRENZY       = 55163,
    SPELL_GROUND_TREMOR     = 55142,
    SPELL_NUMBING_SHOUT     = 55106,
    SPELL_TRANSFORMATION    = 55098,
    SPELL_SUMMON_PHANTOM    = 55205,

    // Mammoth
    SPELL_DETERMINED_GORE   = 55102,
    SPELL_DETERMINED_GORE_H = 59444,
    SPELL_QUAKE             = 55101,
    SPELL_NUMBING_ROAR      = 55100,

    MOORABI_SPELL_LIST_NORMAL  = 0,
    MOORABI_SPELL_LIST_MAMMOTH = 0,
};

enum MoorabiActions
{
    MOORABI_TRANSFORM,
    MOORABI_ACTION_MAX,
};

/*######
## boss_moorabi
######*/

struct boss_moorabiAI : public CombatAI
{
    boss_moorabiAI(Creature* creature) : CombatAI(creature, MOORABI_ACTION_MAX), m_instance(static_cast<instance_gundrak*>(creature->GetInstanceData())),
                                         m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(MOORABI_TRANSFORM, 10000u);
        Reset();
    }

    instance_gundrak* m_instance;
    bool m_bIsRegularMode;

    uint32 m_previousTimer;

    void Reset() override
    {
        CombatAI::Reset();

        m_creature->SetSpellList(MOORABI_SPELL_LIST_NORMAL);
        m_previousTimer = 10000;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_MOJO_FRENZY);

        if (m_instance)
            m_instance->SetData(TYPE_MOORABI, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MOORABI, DONE);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo, SpellMissInfo /*missInfo*/) override
    {
        if (spellInfo->Id == SPELL_TRANSFORMATION)
        {
            DoScriptText(EMOTE_TRANSFORMED, m_creature);

            // Set the achievement to failed
            if (m_instance)
                m_instance->SetLessRabiAchievementCriteria(false);

            m_creature->SetSpellList(MOORABI_SPELL_LIST_MAMMOTH);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == MOORABI_TRANSFORM)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_TRANSFORMATION) == CAST_OK)
            {
                DoScriptText(SAY_TRANSFORM, m_creature);
                DoScriptText(EMOTE_TRANSFORM, m_creature);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_PHANTOM);
                m_previousTimer *= 0.8;
                ResetTimer(action, m_previousTimer);
            }
        }
    }
};

void AddSC_boss_moorabi()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_moorabi";
    pNewScript->GetAI = &GetNewAIInstance<boss_moorabiAI>;
    pNewScript->RegisterSelf();
}
