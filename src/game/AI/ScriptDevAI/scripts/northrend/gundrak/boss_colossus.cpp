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
SDName: Boss_Colossus
SD%Complete: 95%
SDComment: Timers; May need small adjustments
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "gundrak.h"

enum
{
    EMOTE_SURGE                 = -1604008,
    EMOTE_SEEP                  = -1604009,
    EMOTE_GLOW                  = -1604010,

    // collosus' abilities
    SPELL_FREEZE_ANIM           = 16245,                // Colossus stun aura
    SPELL_EMERGE                = 54850,
    SPELL_MIGHTY_BLOW           = 54719,
    SPELL_MORTAL_STRIKES        = 54715,
    SPELL_MORTAL_STRIKES_H      = 59454,

    // elemental's abilities
    SPELL_MERGE                 = 54878,
    SPELL_SURGE                 = 54801,
    SPELL_MOJO_VOLLEY           = 59453,
    SPELL_MOJO_VOLLEY_H         = 54849,

    // Living Mojo spells
    SPELL_MOJO_WAVE             = 55626,
    SPELL_MOJO_WAVE_H           = 58993,
    SPELL_MOJO_PUDDLE           = 55627,
    SPELL_MOJO_PUDDLE_H         = 58994,

    MAX_COLOSSUS_MOJOS          = 5,
};

/*######
## boss_drakkari_elemental
######*/

enum ElementalActions
{
    ELEMENTAL_MOJO_VOLLEY,
    ELEMENTAL_MERGE,
    ELEMENTAL_ACTIONS_MAX,
};

struct boss_drakkari_elementalAI : public BossAI
{
    boss_drakkari_elementalAI(Creature* creature) : BossAI(creature, ELEMENTAL_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(ELEMENTAL_MOJO_VOLLEY, true);
        AddTimerlessCombatAction(ELEMENTAL_MERGE, true);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;

    void JustReachedHome() override
    {
        m_creature->ForcedDespawn();

        if (!m_instance)
            return;

        if (Creature* colossus = m_instance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
            colossus->AI()->EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!m_instance)
            return;
        // kill colossus on death - this will finish the encounter
        if (Creature* colossus = m_instance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
            colossus->Suicide();
    }

    // Set the second emerge of the Elemental
    void DoPrepareSecondEmerge()
    {
        DisableCombatAction(ELEMENTAL_MERGE);
        m_creature->SetHealth(m_creature->GetMaxHealth() * 0.5f);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ELEMENTAL_MERGE:
            {
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    DoCastSpellIfCan(nullptr, SPELL_MERGE, CAST_INTERRUPT_PREVIOUS);
                    DisableCombatAction(action);
                }
                return;
            }
            case ELEMENTAL_MOJO_VOLLEY:
            {
                if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_MOJO_VOLLEY : SPELL_MOJO_VOLLEY_H) == CAST_OK)
                    DisableCombatAction(action);
                return;
            }
        }
    }
};

/*######
## boss_drakkari_colossus
######*/

enum ColossusActions
{
    COLOSSUS_MORTAL_STRIKES,
    COLOSSUS_EMERGE,
    COLOSSUS_START_COMBAT,
    COLOSSUS_ACTIONS_MAX,
};

struct boss_drakkari_colossusAI : public BossAI
{
    boss_drakkari_colossusAI(Creature* creature) : BossAI(creature, COLOSSUS_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_COLOSSUS);
        AddTimerlessCombatAction(COLOSSUS_MORTAL_STRIKES, true);
        AddTimerlessCombatAction(COLOSSUS_EMERGE, true);
        AddCustomAction(COLOSSUS_START_COMBAT, true, [&]()
        {
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        });
        SetDeathPrevention(true);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;
    bool m_firstEmerge;

    uint8 m_mojosGathered;

    void Reset() override
    {
        BossAI::Reset();
        m_firstEmerge     = true;
        m_mojosGathered   = 0;

        // Reset unit flags
        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_MERGE)
        {
            // re-activate colossus here
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);

            SetCombatMovement(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
            static_cast<Creature*>(caster)->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ELEMENTAL)
        {
            // If this is the second summon, then set the health to half
            if (!m_firstEmerge)
            {
                if (boss_drakkari_elementalAI* elementalAI = dynamic_cast<boss_drakkari_elementalAI*>(summoned->AI()))
                    elementalAI->DoPrepareSecondEmerge();
            }

            m_firstEmerge = false;
            if (m_creature->GetVictim())
                summoned->AI()->AttackStart(m_creature->GetVictim());
        }
    }

    void DoEmergeElemental()
    {
        // Avoid casting the merge spell twice
        if (m_creature->HasAura(SPELL_FREEZE_ANIM))
            return;

        if (DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
        {
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            DoCastSpellIfCan(nullptr, SPELL_FREEZE_ANIM, CAST_TRIGGERED);
        }
    }

    // Wrapper to prepare the Colossus
    void DoPrepareColossus()
    {
        ++m_mojosGathered;

        if (m_mojosGathered == MAX_COLOSSUS_MOJOS)
            ResetTimer(COLOSSUS_START_COMBAT, 1s);
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        DoEmergeElemental();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case COLOSSUS_EMERGE:
            {
                if (m_firstEmerge && m_creature->GetHealthPercent() < 50.0f)
                {
                    DoEmergeElemental();
                    DisableCombatAction(action);
                }
                return;
            }
            case COLOSSUS_MORTAL_STRIKES:
            {
                DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_MORTAL_STRIKES : SPELL_MORTAL_STRIKES_H);
                DisableCombatAction(action);
                return;
            }
        }
    }
};

/*######
## npc_living_mojo
######*/

enum LivingMojoActions
{
    MOJO_MOJO_WAVE,
    MOJO_ACTIONS_MAX,
};

struct npc_living_mojoAI : public CombatAI
{
    npc_living_mojoAI(Creature* creature) : CombatAI(creature, MOJO_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_isPartOfColossus(creature->GetPositionX() > 1650.0f)
    {
        AddCombatAction(MOJO_MOJO_WAVE, 10s, 13s);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;
    bool m_isPartOfColossus;

    void AttackStart(Unit* who) override
    {
        // Don't attack if is part of the Colossus event
        if (m_isPartOfColossus)
            return;

        CombatAI::AttackStart(who);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (!pointId)
            return;

        m_creature->ForcedDespawn(std::chrono::milliseconds(1s).count());

        if (!m_instance)
            return;

        // Prepare to set the Colossus in combat
        if (Creature* colossus = m_instance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
        {
            if (boss_drakkari_colossusAI* colossusAI = dynamic_cast<boss_drakkari_colossusAI*>(colossus->AI()))
                colossusAI->DoPrepareColossus();
        }
    }

    void EnterEvadeMode() override
    {
        if (!m_isPartOfColossus)
        {
            CombatAI::EnterEvadeMode();
            return;
        }

        // Force the Mojo to move to the Colossus position
        if (!m_instance)
            return;

        float fX, fY, fZ;
        m_creature->GetPosition(fX, fY, fZ);

        if (Creature* colossus = m_instance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
            colossus->GetPosition(fX, fY, fZ);

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_MOJO_PUDDLE : SPELL_MOJO_PUDDLE_H, CAST_TRIGGERED);
        if (!m_instance || !m_isPartOfColossus)
            return;

        // Prepare to set the Colossus in combat
        if (Creature* colossus = m_instance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
        {
            if (boss_drakkari_colossusAI* colossusAI = dynamic_cast<boss_drakkari_colossusAI*>(colossus->AI()))
                colossusAI->DoPrepareColossus();
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == MOJO_MOJO_WAVE && m_isPartOfColossus)
            DisableCombatAction(action);
        else if (action == MOJO_MOJO_WAVE && !m_isPartOfColossus)
        {
            if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_MOJO_WAVE : SPELL_MOJO_WAVE_H) == CAST_OK)
            {
                ResetCombatAction(action, std::chrono::seconds(urand(15,18)));
                return;
            }
        }
    }
};

void AddSC_boss_colossus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_drakkari_colossus";
    pNewScript->GetAI = &GetNewAIInstance<boss_drakkari_colossusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_drakkari_elemental";
    pNewScript->GetAI = &GetNewAIInstance<boss_drakkari_elementalAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_mojo";
    pNewScript->GetAI = &GetNewAIInstance<npc_living_mojoAI>;
    pNewScript->RegisterSelf();
}
