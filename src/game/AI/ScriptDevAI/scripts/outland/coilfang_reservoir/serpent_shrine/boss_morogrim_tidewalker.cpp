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
SDName: Boss_Morogrim_Tidewalker
SD%Complete: 90
SDComment: Water Globule script is not complete - requires additional research.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                       = -1548030,
    SAY_SUMMON1                     = -1548031,
    SAY_SUMMON2                     = -1548032,
    SAY_SUMMON_BUBL1                = -1548033,
    SAY_SUMMON_BUBL2                = -1548034,
    SAY_SLAY1                       = -1548035,
    SAY_SLAY2                       = -1548036,
    SAY_SLAY3                       = -1548037,
    SAY_DEATH                       = -1548038,
    EMOTE_WATERY_GRAVE              = -1548039,
    EMOTE_EARTHQUAKE                = -1548040,
    EMOTE_WATERY_GLOBULES           = -1548041,

    SPELL_DOUBLE_ATTACK             = 18943,
    SPELL_TIDAL_WAVE                = 37730,
    SPELL_EARTHQUAKE                = 37764,
    SPELL_WATERY_GRAVE              = 38028,
    SPELL_WATERY_GRAVE_TELEPORT_1   = 37850,
    SPELL_WATERY_GRAVE_TELEPORT_2   = 38023,
    SPELL_WATERY_GRAVE_TELEPORT_3   = 38024,
    SPELL_WATERY_GRAVE_TELEPORT_4   = 38025,
    // SPELL_WATERY_GRAVE_EXPLOSION  = 38049,               // spell purpose unk

    SPELL_SUMMON_MURLOC_A1          = 37766,
    SPELL_SUMMON_MURLOC_A2          = 37923,
    SPELL_SUMMON_MURLOC_A3          = 37925,
    SPELL_SUMMON_MURLOC_A4          = 37926,
    SPELL_SUMMON_MURLOC_A5          = 37927,
    SPELL_SUMMON_MURLOC_A6          = 39813,
    SPELL_SUMMON_MURLOC_A7          = 39814,
    SPELL_SUMMON_MURLOC_A8          = 39815,
    SPELL_SUMMON_MURLOC_A9          = 39816,
    SPELL_SUMMON_MURLOC_A10         = 39817,

    SPELL_SUMMON_MURLOC_B1          = 37927,
    SPELL_SUMMON_MURLOC_B2          = 37928,
    SPELL_SUMMON_MURLOC_B3          = 37929,
    SPELL_SUMMON_MURLOC_B4          = 37931,
    SPELL_SUMMON_MURLOC_B5          = 37932,
    SPELL_SUMMON_MURLOC_B6          = 39818,
    SPELL_SUMMON_MURLOC_B7          = 39819,
    SPELL_SUMMON_MURLOC_B8          = 39820,
    SPELL_SUMMON_MURLOC_B9          = 39821,
    SPELL_SUMMON_MURLOC_B10         = 39822,

    SPELL_SUMMON_GLOBULE_1          = 37854,
    SPELL_SUMMON_GLOBULE_2          = 37858,
    SPELL_SUMMON_GLOBULE_3          = 37860,
    SPELL_SUMMON_GLOBULE_4          = 37861,

    // Water Globule
    SPELL_WATER_GLOBULE_EXPLODE     = 37871,                // Used when the globule reaches within 5y of the players
    SPELL_WATER_GLOBULE_NEW_TARGET  = 39848,                // spell requires additional research and probably core or script support

    NPC_WATER_GLOBULE               = 21913,
    NPC_TIDEWALKER_LURKER           = 21920
};

static const uint32 m_auiSpellWateryGraveTeleport[] = { SPELL_WATERY_GRAVE_TELEPORT_1, SPELL_WATERY_GRAVE_TELEPORT_2, SPELL_WATERY_GRAVE_TELEPORT_3, SPELL_WATERY_GRAVE_TELEPORT_4 };
static const uint32 m_auiSpellSummonGlobule[] = { SPELL_SUMMON_GLOBULE_1, SPELL_SUMMON_GLOBULE_2, SPELL_SUMMON_GLOBULE_3, SPELL_SUMMON_GLOBULE_4 };

enum MorogrimActions
{
    MOROGRIM_PHASE_2,
    MOROGRIM_EARTHQUAKE,
    MOROGRIM_TIDAL_WAVE,
    MOROGRIM_WATERY_GRAVE,
    MOROGRIM_WATER_GLOBULES,
    MOROGRIM_ACTION_MAX,
};

struct boss_morogrim_tidewalkerAI : public CombatAI
{
    boss_morogrim_tidewalkerAI(Creature* creature) : CombatAI(creature, MOROGRIM_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MOROGRIM_PHASE_2, true);
        AddCombatAction(MOROGRIM_EARTHQUAKE, 40000u);
        AddCombatAction(MOROGRIM_TIDAL_WAVE, 10000, 15000);
        AddCombatAction(MOROGRIM_WATERY_GRAVE, 30000u);
        AddCombatAction(MOROGRIM_WATER_GLOBULES, true);
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float /*y*/, float /*z*/)
        {
            return x < 304.12f || x > 457.35f;
        });
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MOROGRIM_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MOROGRIM_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MOROGRIM_EVENT, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_TIDEWALKER_LURKER)
        {
            summoned->SetNoCallAssistance(true);
            summoned->SetInCombatWithZone();
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                summoned->AI()->AttackStart(target);
            summoned->SetCorpseDelay(2);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MOROGRIM_PHASE_2:
            {
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    DisableCombatAction(MOROGRIM_WATERY_GRAVE);
                    ResetCombatAction(MOROGRIM_WATER_GLOBULES, 1);
                    SetActionReadyStatus(action, false);
                }
                break;
            }
            case MOROGRIM_EARTHQUAKE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_EARTHQUAKE) == CAST_OK)
                {
                    DoScriptText(EMOTE_EARTHQUAKE, m_creature);
                    DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);

                    // summon murlocs - north
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A1, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A3, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A5, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A7, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A9, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_A10, CAST_TRIGGERED);

                    // summon murlocs - south
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B2, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B4, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B6, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B8, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B9, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_MURLOC_B10, CAST_TRIGGERED);

                    ResetCombatAction(action, 50000);
                }
                break;
            }
            case MOROGRIM_TIDAL_WAVE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_TIDAL_WAVE) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 25000));
                break;
            }
            case MOROGRIM_WATERY_GRAVE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_WATERY_GRAVE) == CAST_OK)
                {
                    DoScriptText(EMOTE_WATERY_GRAVE, m_creature);
                    ResetCombatAction(action, 30000);
                }
                break;
            }
            case MOROGRIM_WATER_GLOBULES:
            {
                if (!CanExecuteCombatAction())
                    return;

                for (uint8 i = 0; i < 4; ++i)
                    m_creature->CastSpell(nullptr, m_auiSpellSummonGlobule[i], TRIGGERED_OLD_TRIGGERED);

                DoScriptText(urand(0, 1) ? SAY_SUMMON_BUBL1 : SAY_SUMMON_BUBL2, m_creature);
                DoScriptText(EMOTE_WATERY_GLOBULES, m_creature);
                ResetCombatAction(action, 25000);
                break;
            }
        }
    }
};

struct mob_water_globuleAI : public ScriptedAI
{
    mob_water_globuleAI(Creature* creature) : ScriptedAI(creature), m_initialAggro(false)
    {
        SetReactState(REACT_DEFENSIVE);
        AddCustomAction(1, 2000u, [&]() { AcquireNewTarget(); });
    }

    bool m_initialAggro;

    void Reset() override
    {
        ResetAllTimers();
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (m_creature->CanAttack(who) && m_creature->IsWithinDist(who, 5.f))
        {
            DoCastSpellIfCan(who, SPELL_WATER_GLOBULE_EXPLODE, TRIGGERED_OLD_TRIGGERED);
            m_creature->ForcedDespawn();
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_WATER_GLOBULE_NEW_TARGET)
        {
            m_creature->AddThreat(target, 3000000.0f);
            DoStartMovement(target);
        }
    }

    void AcquireNewTarget()
    {
        DoResetThreat();
        m_creature->CastSpell(nullptr, SPELL_WATER_GLOBULE_NEW_TARGET, TRIGGERED_NONE);
        ResetTimer(1, urand(7000, 10000));
    }

    void UpdateAI(const uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);
        if (!m_creature->GetVictim() && m_initialAggro)
        {
            AcquireNewTarget();
            return;
        }
    }
};

struct WateryGrave : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target->IsAlive() || spell->GetCaster()->GetVictim() == target)
            return false;

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            uint64 targetIdx = spell->GetScriptValue();
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), m_auiSpellWateryGraveTeleport[targetIdx], TRIGGERED_OLD_TRIGGERED);
            spell->SetScriptValue(targetIdx + 1);
        }
    }
};

void AddSC_boss_morogrim_tidewalker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_morogrim_tidewalker";
    pNewScript->GetAI = &GetNewAIInstance<boss_morogrim_tidewalkerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_water_globule";
    pNewScript->GetAI = &GetNewAIInstance<mob_water_globuleAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<WateryGrave>("spell_watery_grave");
}
