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

#include "precompiled.h"

enum
{
    SPELL_FOUL_SPORES           = 31673,
    SPELL_ACID_GEYSER           = 38739,
    SPELL_DESPAWN_MUSHROOMS     = 34874,

    // Mushroom spells
    SPELL_SPORE_CLOUD           = 34168,
    SPELL_PUTRID_MUSHROOM       = 31690,
    SPELL_GROW                  = 31698,

    NPC_UNDERBOG_MUSHROOM       = 17990,
};

struct boss_hungarfenAI : public ScriptedAI
{
    boss_hungarfenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    bool m_bHasSpores;
    uint32 m_uiMushroomTimer;
    uint32 m_uiAcidGeyserTimer;

    void Reset() override
    {
        m_bHasSpores        = false;
        m_uiMushroomTimer   = 5000;                         // 1 mushroom after 5s, then one per 10s. This should be different in heroic mode
        m_uiAcidGeyserTimer = 10000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_DESPAWN_MUSHROOMS, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        DoCastSpellIfCan(m_creature, SPELL_DESPAWN_MUSHROOMS, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() <= 20.0f && !m_bHasSpores)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FOUL_SPORES) == CAST_OK)
                m_bHasSpores = true;
        }

        if (m_uiMushroomTimer < uiDiff)
        {
            // Summon a mushroom exactly on target position
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->SummonCreature(NPC_UNDERBOG_MUSHROOM, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);

            m_uiMushroomTimer = m_bIsRegularMode ? 10000 : 5000;
        }
        else
            m_uiMushroomTimer -= uiDiff;

        if (m_uiAcidGeyserTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ACID_GEYSER) == CAST_OK)
                    m_uiAcidGeyserTimer = urand(10000, 17500);
            }
        }
        else
            m_uiAcidGeyserTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hungarfen(Creature* pCreature)
{
    return new boss_hungarfenAI(pCreature);
}

struct mob_underbog_mushroomAI : public ScriptedAI
{
    mob_underbog_mushroomAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiGrowTimer;
    uint32 m_uiShrinkTimer;
    uint32 m_uiSporeTimer;

    void Reset() override
    {
        m_uiGrowTimer   = 1000;
        m_uiSporeTimer  = 15000;
        m_uiShrinkTimer = 20000;

        DoCastSpellIfCan(m_creature, SPELL_PUTRID_MUSHROOM);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { return; }
    void AttackStart(Unit* /*pWho*/) override { return; }

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

CreatureAI* GetAI_mob_underbog_mushroom(Creature* pCreature)
{
    return new mob_underbog_mushroomAI(pCreature);
}

void AddSC_boss_hungarfen()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_hungarfen";
    pNewScript->GetAI = &GetAI_boss_hungarfen;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_underbog_mushroom";
    pNewScript->GetAI = &GetAI_mob_underbog_mushroom;
    pNewScript->RegisterSelf();
}
