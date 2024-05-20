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
SDName: Boss_Warp_Splinter
SD%Complete: 90
SDComment: Timers may need adjustments
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/*#####
# boss_warp_splinter
#####*/

enum
{
    SAY_AGGRO                   = 20024,
    SAY_SLAY_1                  = 20025,
    SAY_SLAY_2                  = 20026,
    SAY_SUMMON_1                = 18376,
    SAY_SUMMON_2                = 20022,
    SAY_DEATH                   = 20028,

    SPELL_WAR_STOMP             = 34716,
    SPELL_SUMMON_SAPLINGS       = 34741,            // this will leech the health from all saplings
    SPELL_ARCANE_VOLLEY         = 36705,
    SPELL_ARCANE_VOLLEY_H       = 39133,

    SPELL_ANCESTRAL_LIFE        = 34742,            // Periodic trigger of 34741
    
    // saplings
    SPELL_MOONFIRE_VISUAL       = 36704,

    NPC_SAPLING                 = 19949,
};

// Summon Saplings spells (too many to declare them above)
// static const uint32 saplingsSummonSpells[10] = {34727, 34730, 34731, 34732, 34733, 34734, 34735, 34736, 34737, 34739};
static const uint32 saplingsSummonSpells[6] = { 34727, 34731, 34733, 34734, 34736, 34739 }; // actually ones used on retail

enum WarpSplinterActions
{
    WARP_SPLINTER_ACTION_MAX,
};

struct boss_warp_splinterAI : public CombatAI
{
    boss_warp_splinterAI(Creature* creature) : CombatAI(creature, WARP_SPLINTER_ACTION_MAX), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    bool m_isRegularMode;

    GuidVector m_saplings;

    void Reset() override
    {
        CombatAI::Reset();
        m_saplings.clear();
        m_creature->RemoveGuardians();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SAPLING)
        {
            m_saplings.push_back(summoned->GetObjectGuid());
            summoned->SetInCombatWithZone();
            summoned->AI()->AttackClosestEnemy();
        }
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
        DespawnGuids(m_saplings);
    }

    // Wrapper to summon all Saplings
    void SummonTreants()
    {
        for (uint32 spellId : saplingsSummonSpells)
            DoCastSpellIfCan(nullptr, spellId, CAST_TRIGGERED);

        DoBroadcastText(urand(0, 1) ? SAY_SUMMON_1 : SAY_SUMMON_2, m_creature);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SUMMON_SAPLINGS: SummonTreants(); break;
        }
    }
};

/*#####
# mob_treant (Sapling)
#####*/
struct npc_saplingAI  : public ScriptedAI
{
    npc_saplingAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override {}

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ANCESTRAL_LIFE)
        {
            SetCombatScriptStatus(true);
            SetCombatMovement(false);
            SetMeleeEnabled(false);
            m_creature->SetTarget(nullptr);
            m_creature->ForcedDespawn(4000);
            DoCastSpellIfCan(nullptr, SPELL_MOONFIRE_VISUAL);
        }
    }
};

void AddSC_boss_warp_splinter()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warp_splinter";
    pNewScript->GetAI = &GetNewAIInstance<boss_warp_splinterAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_warp_splinter_treant";
    pNewScript->GetAI = &GetNewAIInstance<npc_saplingAI>;
    pNewScript->RegisterSelf();
}
