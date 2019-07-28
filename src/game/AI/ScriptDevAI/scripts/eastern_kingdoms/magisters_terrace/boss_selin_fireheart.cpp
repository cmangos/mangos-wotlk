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
SDName: Boss_Selin_Fireheart
SD%Complete: 95
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "magisters_terrace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = -1585000,
    SAY_DRAIN_1                     = -1585001, // My hunger knows no bounds!
    SAY_EMPOWERED                   = -1585002,
    SAY_KILL_NORMAL                 = -1585003, // Enough distractions!
    SAY_KILL_EMPOWERED              = -1585004, // I am invincible!
    SAY_DRAIN_2                     = -1585005, // No! More... I must have more!
    EMOTE_CRYSTAL                   = -1585006, // %s begins to channel from the nearby Fel Crystal...

    // Selin's spells
    SPELL_DRAIN_LIFE                = 44294,
    SPELL_DRAIN_LIFE_H              = 46155,
    SPELL_FEL_EXPLOSION             = 44314,
    SPELL_DRAIN_MANA                = 46153,                // Heroic only
    SPELL_FEL_CRYSTAL_DUMMY         = 44329,                // used by Selin to select a nearby Crystal - not used in script
    SPELL_MANA_RAGE_CHANNEL         = 44320,                // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target
    //SPELL_MANA_RAGE_POWER          = 44321,               // Triggered by the channel
    SPELL_DUAL_WEILD                = 42459,

    // Crystal spells
    //SPELL_FEL_CRYSTAL_VISUAL       = 44355,                // cosmetic - defined in instance header
    SPELL_INSTAKILL_SELF            = 29878,

    POINT_CRYSTAL                   = 1,
};

enum SelinActions
{
    SELIN_ACTION_FEL_EXPLOSION,
    SELIN_ACTION_DRAIN_LIFE,
    SELIN_ACTION_DRAIN_MANA,
    SELIN_ACTION_DRAIN_CRYSTAL,
    SELIN_ACTION_MAX,
};

struct boss_selin_fireheartAI : public CombatAI
{
    boss_selin_fireheartAI(Creature* creature) : CombatAI(creature, SELIN_ACTION_MAX), m_instance(static_cast<instance_magisters_terrace*>(creature->GetInstanceData())),
            m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(SELIN_ACTION_FEL_EXPLOSION, 2100u);
        AddCombatAction(SELIN_ACTION_DRAIN_LIFE, 30000, 45000);
        if (!m_isRegularMode)
            AddCombatAction(SELIN_ACTION_DRAIN_MANA, 10000u);
        else
            AddCombatAction(SELIN_ACTION_DRAIN_MANA, true);
        AddCombatAction(SELIN_ACTION_DRAIN_CRYSTAL, 15000, 25000);
        m_creature->SetWalk(false);
        Reset();
    }

    instance_magisters_terrace* m_instance;
    bool m_isRegularMode;
    bool m_empowered;

    ObjectGuid m_crystalGuid;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatScriptStatus(false);
        m_empowered = false;

        DoCastSpellIfCan(nullptr, SPELL_DUAL_WEILD, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_creature->HandleEmote(EMOTE_STATE_STUN);
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case SELIN_ACTION_FEL_EXPLOSION: return 2000;
            case SELIN_ACTION_DRAIN_LIFE: return urand(12000, 22000);
            case SELIN_ACTION_DRAIN_MANA: return urand(17500, 25000);
            case SELIN_ACTION_DRAIN_CRYSTAL: return m_isRegularMode ? urand(40000, 55000) : urand(30000, 40000);
            default: return 0; // never occurs but for compiler
        }
    }

    // Get the nearest alive crystal for draining
    bool DoSelectNearestCrystal()
    {
        // Wait to finish casting
        if (m_creature->IsNonMeleeSpellCasted(false))
            return false;

        if (Creature* crystal = GetClosestCreatureWithEntry(m_creature, NPC_FEL_CRYSTAL, 60.0f))
        {
            m_crystalGuid = crystal->GetObjectGuid();
            DoScriptText(urand(0, 1) ? SAY_DRAIN_1 : SAY_DRAIN_2, m_creature);

            float x, y, z;
            m_creature->GetContactPoint(crystal, x, y, z, INTERACTION_DISTANCE);
            m_creature->GetMotionMaster()->MovePoint(POINT_CRYSTAL, x, y, z);
            SetCombatScriptStatus(true);
            SetMeleeEnabled(false);
            m_creature->SetTarget(nullptr);
            SetCombatMovement(false, true);

            return true;
        }

        return false;
    }

    void DoEndCrystalDraining()
    {
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);
        SetCombatMovement(true, true);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->HandleEmote(0);

        if (m_instance)
            m_instance->SetData(TYPE_SELIN, IN_PROGRESS);
    }

    void JustRespawned() override
    {
        if (m_instance)
            m_instance->StartCrystalVisual();
    }

    void JustReachedHome() override
    {
        m_creature->SetPower(POWER_MANA, 0);

        if (m_instance)
            m_instance->SetData(TYPE_SELIN, FAIL);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        DoScriptText(m_empowered ? SAY_KILL_EMPOWERED : SAY_KILL_NORMAL, m_creature);
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || pointId != POINT_CRYSTAL)
            return;

        bool castSuccessful = false;
        if (DoCastSpellIfCan(nullptr, SPELL_FEL_CRYSTAL_DUMMY) == CAST_OK)
        {
            DoScriptText(EMOTE_CRYSTAL, m_creature);
            return;
        }

        // Make an error message in case something weird happened here
        script_error_log("Selin Fireheart unable to drain crystal as the crystal is either dead or deleted..");
        DoEndCrystalDraining(); // Just in case
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SELIN, DONE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 miscValue) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A: 
                if (bool(miscValue)) // Channeling Completed
                {
                    DoScriptText(SAY_EMPOWERED, m_creature);
                    m_empowered = true;
                    if (invoker->isAlive()) // Kill crystal
                        invoker->CastSpell(nullptr, SPELL_INSTAKILL_SELF, TRIGGERED_OLD_TRIGGERED);
                }

                DoEndCrystalDraining();
            break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SELIN_ACTION_FEL_EXPLOSION:
            {
                DoCastSpellIfCan(nullptr, SPELL_FEL_EXPLOSION);
                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case SELIN_ACTION_DRAIN_LIFE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DRAIN_LIFE, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(target, m_isRegularMode ? SPELL_DRAIN_LIFE : SPELL_DRAIN_LIFE_H);

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case SELIN_ACTION_DRAIN_MANA:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DRAIN_MANA, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                    DoCastSpellIfCan(target, SPELL_DRAIN_MANA);

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case SELIN_ACTION_DRAIN_CRYSTAL:
            {
                if (DoSelectNearestCrystal())
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
        }
    }
};

UnitAI* GetAI_boss_selin_fireheart(Creature* creature)
{
    return new boss_selin_fireheartAI(creature);
};

struct mob_fel_crystalAI : public ScriptedAI
{
    mob_fel_crystalAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override 
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
    }

    void JustRespawned() override
    {
        m_creature->CastSpell(nullptr, SPELL_FEL_CRYSTAL_VISUAL, TRIGGERED_NONE);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_FEL_CRYSTAL_DUMMY)
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
};

UnitAI* GetAI_mob_fel_crystal(Creature* creature)
{
    return new mob_fel_crystalAI(creature);
};

void AddSC_boss_selin_fireheart()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_selin_fireheart";
    pNewScript->GetAI = &GetAI_boss_selin_fireheart;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_crystal";
    pNewScript->GetAI = &GetAI_mob_fel_crystal;
    pNewScript->RegisterSelf();
}
