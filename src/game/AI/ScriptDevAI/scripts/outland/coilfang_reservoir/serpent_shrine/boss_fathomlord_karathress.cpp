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
SDName: Boss_Fathomlord_Karathress
SD%Complete: 95
SDComment: Timers may need adjustments.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                       = -1548021,
    SAY_GAIN_BLESSING               = -1548022,
    SAY_GAIN_ABILITY1               = -1548023,
    SAY_GAIN_ABILITY2               = -1548024,
    SAY_GAIN_ABILITY3               = -1548025,
    SAY_SLAY1                       = -1548026,
    SAY_SLAY2                       = -1548027,
    SAY_SLAY3                       = -1548028,
    SAY_DEATH                       = -1548029,

    // Karathress spells
    SPELL_CATACLYSMIC_BOLT          = 38441,
    SPELL_ENRAGE                    = 24318,                // ToDo: spell needs to be confirmed
    SPELL_SEAR_NOVA                 = 38445,
    SPELL_BLESSING_OF_THE_TIDES     = 38449,                // cast by each of the advisors when the boss reaches 75% hp

    // Sharkkis spells
    SPELL_LEECHING_THROW            = 29436,
    SPELL_BESTIAL_WRATH             = 38371,                // pet has this during 38373
    SPELL_THE_BEAST_WITHIN          = 38373,
    SPELL_HURL_TRIDENT              = 38374,
    SPELL_MULTI_TOSS                = 38366,
    SPELL_SUMMON_FATHOM_LURKER      = 38433,
    SPELL_SUMMON_FATHOM_SPOREBAT    = 38431,
    SPELL_POWER_OF_SHARKKIS         = 38455,                // cast on Karathress, on death
    SPELL_SPAWN_WITH_STUN           = 39795,                // cast on lurker/sporebat on spawn

    // Tidalvess spells
    SPELL_FROST_SHOCK               = 38234,
    SPELL_SPITFIRE_TOTEM            = 38236,
    SPELL_POISON_CLEANSING_TOTEM    = 38306,
    SPELL_EARTHBIND_TOTEM           = 38304,
    SPELL_POWER_OF_TIDALVESS        = 38452,                // cast on Karathress, on death

    // Caribdis Spells
    SPELL_WATER_BOLT_VOLLEY         = 38335,
    SPELL_TIDAL_SURGE               = 38358,                // triggers 38353 which then triggers 38357
    SPELL_HEALING_WAVE              = 38330,
    SPELL_SUMMON_CYCLONE            = 38337,                // summons creature 22104 which uses spell 29538
    SPELL_POWER_OF_CARIBDIS         = 38451,                // cast on Karathress, on death

    SPELL_CYCLONE_WATER_VISUAL_1    = 38464,
    SPELL_CYCLONE_WATER_VISUAL_2    = 38497,
    SPELL_CYCLONE_PERIODIC          = 38516,

    MAX_ADVISORS                    = 3,
    MAX_HEAL_TARGETS                = 4,

    NPC_CYCLONE                     = 22104
};

static const uint32 aAdvisors[MAX_ADVISORS] = {NPC_SHARKKIS, NPC_TIDALVESS, NPC_CARIBDIS};
static const uint32 aHealTargets[MAX_HEAL_TARGETS] = {NPC_KARATHRESS, NPC_CARIBDIS, NPC_SHARKKIS, NPC_TIDALVESS};

/*######
## boss_fathomlord_karathress
######*/

struct boss_fathomlord_karathressAI : public ScriptedAI
{
    boss_fathomlord_karathressAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float /*x*/, float y, float /*z*/)
        {
            return y > -425.f || y < -624.f;
        });
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiCataclysmicBoltTimer;
    uint32 m_uiSearingNovaTimer;
    uint32 m_uiEnrageTimer;

    bool m_bBlessingOfTides;

    GuidVector m_spawns;

    void Reset() override
    {
        m_uiCataclysmicBoltTimer    = 10000;
        m_uiSearingNovaTimer        = urand(20000, 30000);
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;
        m_bBlessingOfTides          = false;

        DespawnGuids(m_spawns);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        switch (pSpell->Id)
        {
            case SPELL_POWER_OF_SHARKKIS:
                DoScriptText(SAY_GAIN_ABILITY1, m_creature);
                break;
            case SPELL_POWER_OF_TIDALVESS:
                DoScriptText(SAY_GAIN_ABILITY1, m_creature);
                break;
            case SPELL_POWER_OF_CARIBDIS:
                DoScriptText(SAY_GAIN_ABILITY3, m_creature);
                break;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KARATHRESS_EVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
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
            m_instance->SetData(TYPE_KARATHRESS_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KARATHRESS_EVENT, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiCataclysmicBoltTimer < uiDiff)
        {
            // select a random unit with mana other than the main tank
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA);

            // if there aren't other units, cast on the tank
            if (!target)
                target = m_creature->GetVictim();

            if (target)
            {
                if (DoCastSpellIfCan(target, SPELL_CATACLYSMIC_BOLT) == CAST_OK)
                    m_uiCataclysmicBoltTimer = 10000;
            }
        }
        else
            m_uiCataclysmicBoltTimer -= uiDiff;

        if (!m_bBlessingOfTides && m_creature->GetHealthPercent() < 75.0f)
        {
            for (unsigned int aAdvisor : aAdvisors)
            {
                if (Creature* pAdvisor = m_instance->GetSingleCreatureFromStorage(aAdvisor))
                {
                    // stack max three times (one for each alive)
                    if (pAdvisor->IsAlive())
                    {
                        pAdvisor->InterruptNonMeleeSpells(false);
                        pAdvisor->CastSpell(m_creature, SPELL_BLESSING_OF_THE_TIDES, TRIGGERED_OLD_TRIGGERED);
                    }
                }
            }

            // yell if we now have the aura
            if (m_creature->HasAura(SPELL_BLESSING_OF_THE_TIDES))
                DoScriptText(SAY_GAIN_BLESSING, m_creature);

            m_bBlessingOfTides = true;
        }

        if (m_uiSearingNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SEAR_NOVA) == CAST_OK)
                m_uiSearingNovaTimer = urand(20000, 30000);
        }
        else
            m_uiSearingNovaTimer -= uiDiff;

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                    m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_fathomguard_sharkkis
######*/

enum SharkkisActions
{
    SHARKKIS_HURL_TRIDENT,
    SHARKKIS_LEECHING_THROW,
    SHARKKIS_THE_BEAST_WITHIN,
    SHARKKIS_MULTI_TOSS,
    SHARKKIS_PET_SUMMON,
    SHARKKIS_ACTION_MAX,
};

struct boss_fathomguard_sharkkisAI : public CombatAI
{
    boss_fathomguard_sharkkisAI(Creature* creature) : CombatAI(creature, SHARKKIS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(SHARKKIS_HURL_TRIDENT, 2500u);
        AddCombatAction(SHARKKIS_LEECHING_THROW, 20000u);
        AddCombatAction(SHARKKIS_THE_BEAST_WITHIN, 30000u);
        AddCombatAction(SHARKKIS_MULTI_TOSS, 7000, 11000);
        AddCombatAction(SHARKKIS_PET_SUMMON, 10000u);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float /*x*/, float y, float /*z*/)
        {
            return y > -425.f || y < -624.f;
        });
        Reset();
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        m_creature->RemoveGuardians(); // despawn pets
    }

    void EnterEvadeMode() override
    {
        if (Creature* karathress = m_instance->GetSingleCreatureFromStorage(NPC_KARATHRESS))
            karathress->AI()->EnterEvadeMode();

        ScriptedAI::EnterEvadeMode();

        Reset();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_SHARKKIS, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (m_creature->GetVictim())
            summoned->AI()->AttackStart(m_creature->GetVictim());

        summoned->CastSpell(summoned, SPELL_SPAWN_WITH_STUN, TRIGGERED_OLD_TRIGGERED);
    }

    void SummonedCreatureJustDied(Creature* /*summoned*/) override
    {
        ResetCombatAction(SHARKKIS_PET_SUMMON, 30000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHARKKIS_HURL_TRIDENT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HURL_TRIDENT, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_HURL_TRIDENT) == CAST_OK)
                        ResetCombatAction(action, 5000);
                break;
            case SHARKKIS_LEECHING_THROW:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_LEECHING_THROW, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                    if (DoCastSpellIfCan(target, SPELL_LEECHING_THROW) == CAST_OK)
                        ResetCombatAction(action, 20000);
                break;
            case SHARKKIS_THE_BEAST_WITHIN:
                if (DoCastSpellIfCan(nullptr, SPELL_THE_BEAST_WITHIN) == CAST_OK) // Personal part on himself
                {
                    m_creature->CastSpell(nullptr, SPELL_BESTIAL_WRATH, TRIGGERED_OLD_TRIGGERED); // Pet part at pet
                    ResetCombatAction(action, 30000);
                }
                break;
            case SHARKKIS_MULTI_TOSS:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_MULTI_TOSS) == CAST_OK)
                        ResetCombatAction(action, urand(7000, 12000));
                break;
            case SHARKKIS_PET_SUMMON:
                if (DoCastSpellIfCan(m_creature, urand(0, 1) ? SPELL_SUMMON_FATHOM_LURKER : SPELL_SUMMON_FATHOM_SPOREBAT) == CAST_OK)
                    DisableCombatAction(action);
                break;
        }
    }
};

/*######
## boss_fathomguard_tidalvess
######*/

enum TidalvessActions
{
    TIDALVESS_TOTEM_SUMMON,
    TIDALVESS_FROST_SHOCK,
    TIDALVESS_ACTION_MAX,
};

struct boss_fathomguard_tidalvessAI : public CombatAI
{
    boss_fathomguard_tidalvessAI(Creature* creature) : CombatAI(creature, TIDALVESS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float /*x*/, float y, float /*z*/)
        {
            return y > -425.f || y < -624.f;
        });
        AddCombatAction(TIDALVESS_TOTEM_SUMMON, 20000, 22000);
        AddCombatAction(TIDALVESS_FROST_SHOCK, 25000u);
        Reset();
    }

    ScriptedInstance* m_instance;

    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();

        DespawnGuids(m_spawns);
    }

    void EnterEvadeMode() override
    {
        if (Creature* karathress = m_instance->GetSingleCreatureFromStorage(NPC_KARATHRESS))
            karathress->AI()->EnterEvadeMode();

        CombatAI::EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_TIDALVESS, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TIDALVESS_TOTEM_SUMMON:
            {
                std::vector<uint32> validSummons;
                if (!m_creature->GetTotem(TOTEM_SLOT_FIRE))
                    validSummons.push_back(SPELL_SPITFIRE_TOTEM);
                if (!m_creature->GetTotem(TOTEM_SLOT_EARTH))
                    validSummons.push_back(SPELL_EARTHBIND_TOTEM);
                if (!m_creature->GetTotem(TOTEM_SLOT_WATER))
                    validSummons.push_back(SPELL_POISON_CLEANSING_TOTEM);
                if (validSummons.size() > 0)
                    if (DoCastSpellIfCan(nullptr, validSummons[urand(0, validSummons.size() - 1)]) == CAST_OK)
                        ResetCombatAction(action, urand(16000, 26000));
                break;
            }
            case TIDALVESS_FROST_SHOCK:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_SHOCK) == CAST_OK)
                    ResetCombatAction(action, urand(25000, 30000));
                break;
        }
    }
};

/*######
## boss_fathomguard_caribdis
######*/

struct boss_fathomguard_caribdisAI : public ScriptedAI
{
    boss_fathomguard_caribdisAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float /*x*/, float y, float /*z*/)
        {
            return y > -425.f || y < -624.f;
        });
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiWaterBoltVolleyTimer;
    uint32 m_uiTidalSurgeTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiCycloneTimer;

    GuidVector m_spawns;

    void Reset() override
    {
        m_uiWaterBoltVolleyTimer = 35000;
        m_uiTidalSurgeTimer      = urand(15000, 20000);
        m_uiHealTimer            = 20000;
        m_uiCycloneTimer         = urand(10000, 15000);

        DespawnGuids(m_spawns);
    }

    void EnterEvadeMode() override
    {
        if (Creature* karathress = m_instance->GetSingleCreatureFromStorage(NPC_KARATHRESS))
            karathress->AI()->EnterEvadeMode();

        ScriptedAI::EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_CARIBDIS, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiWaterBoltVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WATER_BOLT_VOLLEY) == CAST_OK)
                m_uiWaterBoltVolleyTimer = 30000;
        }
        else
            m_uiWaterBoltVolleyTimer -= uiDiff;

        if (m_uiTidalSurgeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TIDAL_SURGE) == CAST_OK)
                m_uiTidalSurgeTimer = urand(15000, 20000);
        }
        else
            m_uiTidalSurgeTimer -= uiDiff;

        if (m_uiCycloneTimer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SUMMON_CYCLONE, SELECT_FLAG_PLAYER))
                if (DoCastSpellIfCan(target, SPELL_SUMMON_CYCLONE) == CAST_OK)
                    m_uiCycloneTimer = urand(45000, 60000);
        }
        else
            m_uiCycloneTimer -= uiDiff;

        if (m_uiHealTimer < uiDiff)
        {
            for (uint8 i = 0; i < MAX_HEAL_TARGETS; ++i)
            {
                if (Creature* pHealTarget = m_instance->GetSingleCreatureFromStorage(aHealTargets[i]))
                {
                    if (pHealTarget->IsAlive() && pHealTarget->GetHealthPercent() < 50.0f)
                    {
                        if (DoCastSpellIfCan(pHealTarget, SPELL_HEALING_WAVE) == CAST_OK)
                        {
                            m_uiHealTimer = urand(12000, 22000);
                            break;
                        }
                    }
                }
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct npc_caribdis_cyclone : public CombatAI
{
    npc_caribdis_cyclone(Creature* creature) : CombatAI(creature, 0)
    {
        AddCustomAction(1, true, [&]() { HandleTargetPick(); });
    }

    void HandleTargetPick()
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
            m_creature->AddThreat(target, 1000000.f);
        ResetTimer(1, 10000);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_CYCLONE_WATER_VISUAL_1, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_CYCLONE_WATER_VISUAL_2, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_CYCLONE_PERIODIC, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_creature->SetInCombatWithZone();
        m_creature->SetWalk(false);
        HandleTargetPick();
    }

    void ExecuteAction(uint32 /*action*/) override {}
};

struct TidalSurge : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->GetUnitTarget() && effIdx == EFFECT_INDEX_1)
            spell->GetUnitTarget()->CastSpell(nullptr, 38353, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_fathomlord_karathress()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_fathomlord_karathress";
    pNewScript->GetAI = &GetNewAIInstance<boss_fathomlord_karathressAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_fathomguard_sharkkis";
    pNewScript->GetAI = &GetNewAIInstance<boss_fathomguard_sharkkisAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_fathomguard_tidalvess";
    pNewScript->GetAI = &GetNewAIInstance<boss_fathomguard_tidalvessAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_fathomguard_caribdis";
    pNewScript->GetAI = &GetNewAIInstance<boss_fathomguard_caribdisAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_caribdis_cyclone";
    pNewScript->GetAI = &GetNewAIInstance<npc_caribdis_cyclone>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TidalSurge>("spell_tidal_surge_caribdis");
}
