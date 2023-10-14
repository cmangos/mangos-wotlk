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
SDName: Boss_Aeonus
SD%Complete: 90
SDComment: Small adjustments; Timers
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "dark_portal.h"

enum
{
    SAY_AGGRO               = 16828,
    SAY_BANISH              = 16829,
    SAY_SLAY1               = 16831,
    SAY_SLAY2               = 16832,
    SAY_DEATH               = 16830,
    EMOTE_GENERIC_FRENZY    = 1191,

    SPELL_THRASH            = 8876,
    SPELL_DOUBLE_ATTACK     = 19818,

    SPELL_CLEAVE            = 40504,
    SPELL_TIME_STOP         = 31422,
    SPELL_ENRAGE            = 37605,
    SPELL_SAND_BREATH       = 31473,
    SPELL_SAND_BREATH_H     = 39049
};

struct boss_aeonusAI : public CombatAI
{
    boss_aeonusAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_THRASH : SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
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

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_ENRAGE)
            DoBroadcastText(EMOTE_GENERIC_FRENZY, m_creature);
    }
};

void AddSC_boss_aeonus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_aeonus";
    pNewScript->GetAI = &GetNewAIInstance<boss_aeonusAI>;
    pNewScript->RegisterSelf();
}
