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
SDName: Boss_Pandemonius
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO_1                     = 17771,
    SAY_AGGRO_2                     = 17772,
    SAY_AGGRO_3                     = 17773,
    SAY_KILL_1                      = 17774,
    SAY_KILL_2                      = 17775,
    SAY_DEATH                       = 17776,
    EMOTE_DARK_SHELL                = 21067,

    SPELL_VOID_BLAST                = 32325,
    SPELL_VOID_BLAST_H              = 38760,
    SPELL_DARK_SHELL                = 32358,
    SPELL_DARK_SHELL_H              = 38759,

    MAX_VOID_BLASTS                 = 5,
};

struct boss_pandemoniusAI : public CombatAI
{
    boss_pandemoniusAI(Creature* creature) : CombatAI(creature, 0), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    bool m_isRegularMode;

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        if (spellInfo->Id == SPELL_DARK_SHELL || spellInfo->Id == SPELL_DARK_SHELL_H)
            DoBroadcastText(EMOTE_DARK_SHELL, m_creature);
    }
};

struct VoidBlast : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        uint64 scriptVal = spell->GetScriptValue();
        if (scriptVal == MAX_VOID_BLASTS)
            return;
        SpellCastArgs args;
        args.SetScriptValue(scriptVal + 1);
        caster->CastSpell(args, spell->m_spellInfo, TRIGGERED_NORMAL_COMBAT_CAST | TRIGGERED_IGNORE_COOLDOWNS);
    }
};

void AddSC_boss_pandemonius()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_pandemonius";
    pNewScript->GetAI = &GetNewAIInstance<boss_pandemoniusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<VoidBlast>("spell_void_blast");
}
