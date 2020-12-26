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
SDName: Boss_Warlord_Najentus
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = -1564000,
    SAY_NEEDLE1                     = -1564001,
    SAY_NEEDLE2                     = -1564002,
    SAY_SLAY1                       = -1564003,
    SAY_SLAY2                       = -1564004,
    SAY_TIDAL_BURST_1               = -1564005,
    SAY_TIDAL_BURST_2               = -1564006,
    SAY_TIDAL_BURST_3               = -1564007,
    SAY_ENRAGE                      = -1564008,
    SAY_DEATH                       = -1564009,
    SAY_UNK                         = -1564137,             // Used in videos but no idea on the trigger

    SPELL_CRASHINGWAVE              = 40100,
    SPELL_NEEDLE_SPINE_TARGETING    = 39992,                // Casts 39835 on targets
    SPELL_TIDAL_BURST               = 39878,
    SPELL_TIDAL_SHIELD              = 39872,                // Triggers burst on dispel
    SPELL_IMPALING_SPINE            = 39837,
    SPELL_CREATE_NAJENTUS_SPINE     = 39956,
    SPELL_HURL_SPINE                = 39948,
    SPELL_BERSERK                   = 26662,
    SPELL_DOUBLE_ATTACK             = 19818,

    ITEM_IMPALING_SPINE             = 32408,
};

enum NajentusActions // order based on priority
{
    NAJENTUS_ACTION_TIDAL_SHIELD,
    NAJENTUS_ACTION_ENRAGE,
    NAJENTUS_ACTION_IMPALING_SPINE, // not active during tidal shield
    NAJENTUS_ACTION_NEEDLE_SPINE, // not active during tidal shield
    NAJENTUS_ACTION_MAX
};

struct boss_najentusAI : public CombatAI
{
    boss_najentusAI(Creature* creature) : CombatAI(creature, NAJENTUS_ACTION_MAX), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData()))
    {
        AddCombatAction(NAJENTUS_ACTION_TIDAL_SHIELD, GetInitialActionTimer(NAJENTUS_ACTION_TIDAL_SHIELD));
        AddCombatAction(NAJENTUS_ACTION_ENRAGE, GetInitialActionTimer(NAJENTUS_ACTION_ENRAGE));
        AddCombatAction(NAJENTUS_ACTION_IMPALING_SPINE, GetInitialActionTimer(NAJENTUS_ACTION_IMPALING_SPINE));
        AddCombatAction(NAJENTUS_ACTION_NEEDLE_SPINE, GetInitialActionTimer(NAJENTUS_ACTION_NEEDLE_SPINE));
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float, float)
        {
            return x < 300.f;
        });
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
    }

    instance_black_temple* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        SetCombatMovement(true);

        m_instance->DespawnImpalingSpines();
    }

    uint32 GetInitialActionTimer(NajentusActions id)
    {
        switch (id)
        {
            case NAJENTUS_ACTION_TIDAL_SHIELD: return 60000;
            case NAJENTUS_ACTION_ENRAGE: return 480000;
            case NAJENTUS_ACTION_IMPALING_SPINE: return 20000;
            case NAJENTUS_ACTION_NEEDLE_SPINE: return 10000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(NajentusActions id)
    {
        switch (id)
        {
            case NAJENTUS_ACTION_TIDAL_SHIELD: return 60000;
            case NAJENTUS_ACTION_IMPALING_SPINE: return 20000;
            case NAJENTUS_ACTION_NEEDLE_SPINE: return 3000;
            default: return 0;
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NAJENTUS, NOT_STARTED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NAJENTUS, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        if (m_creature->HasAura(SPELL_TIDAL_SHIELD) && spellInfo->Id == SPELL_HURL_SPINE)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_TIDAL_SHIELD);
            m_creature->CastSpell(nullptr, SPELL_TIDAL_BURST, TRIGGERED_NONE);
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_TIDAL_BURST_1, m_creature); break;
                case 1: DoScriptText(SAY_TIDAL_BURST_2, m_creature); break;
                case 2: DoScriptText(SAY_TIDAL_BURST_3, m_creature); break;
            }
            ResetTimer(NAJENTUS_ACTION_IMPALING_SPINE, GetInitialActionTimer(NAJENTUS_ACTION_IMPALING_SPINE));
            ResetTimer(NAJENTUS_ACTION_NEEDLE_SPINE, GetInitialActionTimer(NAJENTUS_ACTION_NEEDLE_SPINE));
            SetCombatMovement(true);
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NAJENTUS, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);

        auto const& playerList = m_creature->GetMap()->GetPlayers();
        for (auto& mapRef : playerList)
            mapRef.getSource()->DestroyItemCount(ITEM_IMPALING_SPINE, 5, true);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NAJENTUS_ACTION_TIDAL_SHIELD:
                if (DoCastSpellIfCan(nullptr, SPELL_TIDAL_SHIELD) == CAST_OK)
                {
                    DisableCombatAction(NAJENTUS_ACTION_IMPALING_SPINE);
                    DisableCombatAction(NAJENTUS_ACTION_NEEDLE_SPINE);
                    ResetCombatAction(action, GetSubsequentActionTimer(NajentusActions(action)));
                }
                return;
            case NAJENTUS_ACTION_ENRAGE:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_ENRAGE, m_creature);
                    SetActionReadyStatus(action, false);
                }
                return;
            case NAJENTUS_ACTION_IMPALING_SPINE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_IMPALING_SPINE, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK))
                {
                    if (DoCastSpellIfCan(target, SPELL_IMPALING_SPINE) == CAST_OK)
                    {
                        DoScriptText(urand(0, 1) ? SAY_NEEDLE1 : SAY_NEEDLE2, m_creature);
                        ResetCombatAction(action, GetSubsequentActionTimer(NajentusActions(action)));
                    }
                }
                return;
            }
            case NAJENTUS_ACTION_NEEDLE_SPINE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_NEEDLE_SPINE_TARGETING) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(NajentusActions(action)));
                return;
            }
        }
    }
};

void AddSC_boss_najentus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_najentus";
    pNewScript->GetAI = &GetNewAIInstance<boss_najentusAI>;
    pNewScript->RegisterSelf();
}
