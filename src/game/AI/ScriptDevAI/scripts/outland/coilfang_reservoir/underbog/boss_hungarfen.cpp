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
SDName: Boss_Hungarfen
SD%Complete: 80
SDComment: Need confirmation if spell data are same in both modes; The Underbog Mushroom may need some more research
SDCategory: Coilfang Resevoir, Underbog
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SPELL_FOUL_SPORES           = 31673,
    SPELL_ACID_GEYSER           = 38739,
    SPELL_DESPAWN_MUSHROOMS     = 34874,
    SPELL_SUMMON_UNDERBOG_MUSHROOM = 31692,

    // Mushroom spells
    SPELL_SPORE_CLOUD           = 34168,
    SPELL_PUTRID_MUSHROOM       = 31690,
    SPELL_SHRINK                = 31691,
    SPELL_GROW                  = 31698,
};

enum HungarfenActions
{
    HUNGARFEN_FOUL_SPORES,
    HUNGERFEN_SUMMON_SHROOM,
    HUNGARFEN_ACID_GEYSER,
    HUNGARFEN_ACTION_MAX,
};

struct boss_hungarfenAI : public CombatAI
{
    boss_hungarfenAI(Creature* creature) : CombatAI(creature, HUNGARFEN_ACTION_MAX), m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(HUNGARFEN_FOUL_SPORES, true);
        AddCombatAction(HUNGERFEN_SUMMON_SHROOM, 5000u);
        if (!m_bIsRegularMode)
            AddCombatAction(HUNGARFEN_ACID_GEYSER, 10000u);
    }

    bool m_bIsRegularMode;

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(nullptr, SPELL_DESPAWN_MUSHROOMS, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        DoCastSpellIfCan(nullptr, SPELL_DESPAWN_MUSHROOMS, CAST_TRIGGERED);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HUNGARFEN_FOUL_SPORES:
            {
                if (m_creature->GetHealthPercent() <= 20.0f)
                    if (DoCastSpellIfCan(nullptr, SPELL_FOUL_SPORES) == CAST_OK)
                        SetActionReadyStatus(action, false);
                break;
            }
            case HUNGERFEN_SUMMON_SHROOM:
            {
                // Summon a mushroom exactly on target position
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    target->CastSpell(nullptr, SPELL_SUMMON_UNDERBOG_MUSHROOM, TRIGGERED_OLD_TRIGGERED); // should not send packets
                ResetCombatAction(action, m_bIsRegularMode ? 10000 : 5000);
                break;
            }
            case HUNGARFEN_ACID_GEYSER:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_ACID_GEYSER) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 17500));
                break;
            }
        }
    }
};

struct mob_underbog_mushroomAI : public ScriptedAI
{
    mob_underbog_mushroomAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_uiGrowTimer;
    uint32 m_uiShrinkTimer;
    uint32 m_uiSporeTimer;

    void Reset() override
    {
        m_uiGrowTimer   = 1000;
        m_uiSporeTimer  = 15000;
        m_uiShrinkTimer = 20000;
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_PUTRID_MUSHROOM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SHRINK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {
    }
    void AttackStart(Unit* /*pWho*/) override {
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSporeTimer)
        {
            if (m_uiSporeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SPORE_CLOUD) == CAST_OK)
                {
                    m_uiGrowTimer = 0;
                    m_uiSporeTimer = 0;
                }
            }
            else
                m_uiSporeTimer -= uiDiff;
        }

        if (m_uiGrowTimer)
        {
            if (m_uiGrowTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_GROW) == CAST_OK)
                    m_uiGrowTimer = 3000;
            }
            else
                m_uiGrowTimer -= uiDiff;
        }

        if (m_uiShrinkTimer)
        {
            if (m_uiShrinkTimer <= uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_GROW);
                m_uiShrinkTimer = 0;
            }
            else
                m_uiShrinkTimer -= uiDiff;
        }
    }
};

struct DespawnMushrooms : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        static_cast<Creature*>(target)->ForcedDespawn();
    }
};

void AddSC_boss_hungarfen()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hungarfen";
    pNewScript->GetAI = &GetNewAIInstance<boss_hungarfenAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_underbog_mushroom";
    pNewScript->GetAI = &GetNewAIInstance<mob_underbog_mushroomAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DespawnMushrooms>("spell_despawn_underbog_mushrooms");
}
