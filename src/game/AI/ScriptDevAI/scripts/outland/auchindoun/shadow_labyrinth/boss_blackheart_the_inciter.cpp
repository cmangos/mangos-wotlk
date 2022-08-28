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
SDName: Boss_Blackheart_the_Inciter
SD%Complete: 90
SDComment: Not all yells are implemented.
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shadow_labyrinth.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SPELL_CHARGE                = 33709,
    SPELL_WAR_STOMP             = 33707,
    
    // incite chaos mechanic spells
    SPELL_INCITE_CHAOS          = 33676,                       // spawns triggers and roots/stuns blackheart
    SPELL_INCITE_CHAOS_CHARM    = 33684,                       // cast by triggers into players
    SPELL_INCITE_CHAOS_UNK      = 33687,
    SPELL_THREAT                = 34915,                       // Unk purpose, threat is removed right after
    SPELL_INCITER_THREAT_TRIGGER= 45339,
    SPELL_LAUGH                 = 33716,                       // used during incite chaos onto one player every 2-4 seconds TODO: add usage
    SPELL_LAUGH_PERIODIC        = 33722,
    SPELL_LAUGH_UNK             = 33797,

    SPELL_DUMMY_NUKE            = 21912,

    SPELL_INCITE_CHAOS_SPAWN_1  = 33677,
    SPELL_INCITE_CHAOS_SPAWN_2  = 33680,
    SPELL_INCITE_CHAOS_SPAWN_3  = 33681,
    SPELL_INCITE_CHAOS_SPAWN_4  = 33682,
    SPELL_INCITE_CHAOS_SPAWN_5  = 33683,

    SAY_INTRO1              = -1555008,
    SAY_INTRO2              = -1555009,
    SAY_INTRO3              = -1555010,
    SAY_AGGRO1              = -1555011,
    SAY_INCITE_CHAOS        = -1555012,
    SAY_AGGRO2              = -1555013,
    SAY_SLAY1               = 17573,
    SAY_SLAY2               = 19528,
    SAY_HELP                = -1555016,
    SAY_DEATH               = -1555017,

    SAY2_INTRO1             = -1555018,
    SAY2_INTRO2             = -1555019,
    SAY2_INTRO3             = -1555020,
    SAY2_AGGRO1             = -1555021,
    SAY2_AGGRO2             = -1555022,
    SAY2_AGGRO3             = -1555023,
    SAY2_SLAY1              = -1555024,
    SAY2_SLAY2              = -1555025,
    SAY2_HELP               = -1555026,
    SAY2_DEATH              = -1555027,

    // faction IDs used during incite chaos - each trigger has one
    FACTION_INCITER_1       = 1761,
    FACTION_INCITER_2       = 1762,
    FACTION_INCITER_3       = 1763,
    FACTION_INCITER_4       = 1764,
    FACTION_INCITER_5       = 1765,

    // npcs summoned during incite chaos, each one charms a different player - propagates factions above to them
    NPC_INCITER_TRIGGER_1   = 19300, // summoned by spellid 33677
    NPC_INCITER_TRIGGER_2   = 19301, // summoned by spellid 33680
    NPC_INCITER_TRIGGER_3   = 19302, // summoned by spellid 33681
    NPC_INCITER_TRIGGER_4   = 19303, // summoned by spellid 33682
    NPC_INCITER_TRIGGER_5   = 19304, // summoned by spellid 33683
};

enum BlackheartActions
{
    BLACKHEART_INCITE_CHAOS,
    BLACKHEART_CHARGE,
    BLACKHEART_KNOCKBACK,
    BLACKHEART_ACTION_MAX,
    BLACKHEART_INCITE_TIMER,
};

struct boss_blackheart_the_inciterAI : public CombatAI
{
    boss_blackheart_the_inciterAI(Creature* creature) : CombatAI(creature, BLACKHEART_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        if (m_isRegularMode)
            AddCombatAction(BLACKHEART_INCITE_CHAOS, 18000, 24000);
        else
            AddCombatAction(BLACKHEART_INCITE_CHAOS, 12000, 17000);
        AddCombatAction(BLACKHEART_CHARGE, 30000, 50000);
        AddCombatAction(BLACKHEART_KNOCKBACK, 10000, 14000);
        AddCustomAction(BLACKHEART_INCITE_TIMER, true, [&]() { HandleInciteEnd(); });
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;
    GuidVector m_vTargetsGuids;

    uint32 GetSubsequentActionTimer(BlackheartActions id)
    {
        switch (id)
        {
            case BLACKHEART_INCITE_CHAOS: return urand(52000, 60000);
            case BLACKHEART_CHARGE: return urand(26000, 44000);
            case BLACKHEART_KNOCKBACK: return urand(15000, 30000);
            default: return 0;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_INCITER, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 1))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_INCITER, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_INCITER, FAIL);
    }

    void HandleInciteEnd()
    {
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        m_meleeEnabled = true;
        if (m_creature->GetVictim())
        {
            m_creature->MeleeAttackStart(m_creature->GetVictim());
            m_creature->SetTarget(m_creature->GetVictim());
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BLACKHEART_INCITE_CHAOS:
            {
                if (m_creature->getThreatManager().getThreatList().size() > 1)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INCITE_CHAOS) == CAST_OK)
                    {
                        DoScriptText(SAY_INCITE_CHAOS, m_creature);
                        SetCombatScriptStatus(true);
                        SetCombatMovement(false);
                        m_meleeEnabled = false;
                        DoResetThreat();
                        m_creature->MeleeAttackStop(m_creature->GetVictim());
                        m_creature->SetTarget(nullptr);
                        m_creature->CastSpell(nullptr, SPELL_LAUGH_PERIODIC, TRIGGERED_NONE);
                        ResetCombatAction(action, GetSubsequentActionTimer(BlackheartActions(action)));
                        ResetTimer(BLACKHEART_INCITE_TIMER, 19000);
                        return;
                    }
                }
                break;
            }
            case BLACKHEART_CHARGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CHARGE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                {
                    if (DoCastSpellIfCan(target, SPELL_CHARGE) == CAST_OK)
                    {
                        ResetCombatAction(action, GetSubsequentActionTimer(BlackheartActions(action)));
                        DoResetThreat();
                    }
                }
                break;
            }
            case BLACKHEART_KNOCKBACK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_WAR_STOMP) == CAST_OK)
                {
                    ResetCombatAction(action, GetSubsequentActionTimer(BlackheartActions(action)));
                    DoResetThreat();
                }
                break;
            }
        }
    }
};

struct dummy_blackheart_the_inciterAI : public ScriptedAI
{
    dummy_blackheart_the_inciterAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_threat(false),
        m_attack(false)
    {
        SetCombatMovement(false);
    }

    ScriptedInstance* m_instance;
    bool m_threat;
    bool m_attack;

    void Reset() override
    {
    }

    void JustRespawned() override
    {
        if (m_instance)
            if (Creature* blackheartBoss = m_instance->GetSingleCreatureFromStorage(NPC_BLACKHEART_THE_INCITER))
                m_creature->CastSpell(blackheartBoss, SPELL_THREAT, TRIGGERED_NONE);
        m_creature->CastSpell(m_creature->GetSpawner(), SPELL_INCITE_CHAOS_CHARM, TRIGGERED_NONE);

        DoResetThreat();
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_INCITER_THREAT_TRIGGER)
        {
            Unit* myCharm = m_creature->GetCharm();
            Unit* hisCharm = target->GetCharm();
            if (myCharm && hisCharm)
                myCharm->AddThreat(hisCharm);
        }
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
        if (!m_threat)
        {
            m_threat = true;
            m_creature->CastSpell(nullptr, SPELL_INCITER_THREAT_TRIGGER, TRIGGERED_NONE);
        }
        else if (!m_attack)
        {
            m_attack = true;
            if (Unit* charmed = m_creature->GetCharm())
                charmed->AI()->AttackClosestEnemy();
        }

        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DUMMY_NUKE, SELECT_FLAG_PLAYER))
            m_creature->CastSpell(target, SPELL_DUMMY_NUKE, TRIGGERED_NONE);
    }
};

struct InciteChaos : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        uint32 spellId;

        switch (spell->GetScriptValue())
        {
            case 0: spellId = SPELL_INCITE_CHAOS_SPAWN_1; break;
            case 1: spellId = SPELL_INCITE_CHAOS_SPAWN_2; break;
            case 2: spellId = SPELL_INCITE_CHAOS_SPAWN_3; break;
            case 3: spellId = SPELL_INCITE_CHAOS_SPAWN_4; break;
            case 4: spellId = SPELL_INCITE_CHAOS_SPAWN_5; break;
        }

        spell->SetScriptValue(spell->GetScriptValue() + 1);

        unitTarget->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED); // shouldnt be sent to client
    }
};

void AddSC_boss_blackheart_the_inciter()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_blackheart_the_inciter";
    pNewScript->GetAI = &GetNewAIInstance<boss_blackheart_the_inciterAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "dummy_blackheart_the_inciter";
    pNewScript->GetAI = &GetNewAIInstance<dummy_blackheart_the_inciterAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<InciteChaos>("spell_incite_chaos");
}
