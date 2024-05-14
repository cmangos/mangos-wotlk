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
SDName: Boss_Hydross_The_Unstable
SD%Complete: 95
SDComment: Timers may need improvemets.
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

// Note: As of March 21 2007 Hydross should not crush tanks

enum
{
    SAY_AGGRO                   = 20231,
    SAY_SWITCH_TO_CLEAN         = 19607,
    SAY_CLEAN_SLAY1             = 20235,
    SAY_CLEAN_SLAY2             = 20236,
    SAY_CLEAN_DEATH             = 20238,
    SAY_SWITCH_TO_CORRUPT       = 19606,
    SAY_CORRUPT_SLAY1           = 20233,
    SAY_CORRUPT_SLAY2           = 20234,
    SAY_CORRUPT_DEATH           = 20239,

    SPELL_WATER_TOMB            = 38235,
    SPELL_VILE_SLUDGE           = 38246,
    SPELL_CORRUPTION_SD         = 37961,                    // transform spell
    SPELL_BERSERK               = 27680,                    // ToDo: this spell need verification
    SPELL_BLUE_BEAM             = 38015,
    SPELL_SUMMON_WATER_ELEMENTAL  = 36459,                    // spawn elemental on OOC timer
    SPELL_ELEMENTAL_SPAWNIN     = 25035,
    SPELL_PURIFY_ELEMENTAL      = 36461,                    // purify elemental on OOC timer
    SPELL_CLEANSING_FIELD       = 37935,

    NPC_PURE_SPAWN              = 22035,
    NPC_TAINTED_SPAWN           = 22036,
    NPC_PURIFIED_ELEMENTAL      = 21260,
    NPC_TAINTED_ELEMENTAL       = 21253,
    NPC_HYDROSS_THE_UNSTABLE    = 21216,

    SWITCH_RADIUS               = 20,
    MAX_HYDROSS_ADDS            = 4,
    MAX_HYDROSS_MARKS           = 6,

    SPELL_LIST_CLEAN_INITIAL    = 2121601,
    SPELL_LIST_CORRUPT          = 2121602,
    SPELL_LIST_CLEAN_REPEAT     = 2121603, // does not incur berserk cooldown
};

static const uint32 aMarkHydross[MAX_HYDROSS_MARKS] = {38215, 38216, 38217, 38218, 38231, 40584};
static const uint32 aMarkCorruption[MAX_HYDROSS_MARKS] = {38219, 38220, 38221, 38222, 38230, 40583};

static const uint32 purifiedSpawns[] = { 38198, 38199, 38200, 38201 };
static const uint32 corruptedSpawns[] = { 38188, 38189, 38190, 38191 };

enum HydrossActions
{
    HYDROSS_SWITCH,
    HYDROSS_ACTION_MAX,
    HYDROSS_BEAM_INIT,
    HYDROSS_SPAWN_ELEMENTAL
};

struct boss_hydross_the_unstableAI : public CombatAI
{
    boss_hydross_the_unstableAI(Creature* creature) : CombatAI(creature, HYDROSS_ACTION_MAX),
        m_instance(static_cast<instance_serpentshrine_cavern*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(HYDROSS_SWITCH, true);
        AddCustomAction(HYDROSS_BEAM_INIT, 5000u, [&]() { HandleBeamHelpers(false); }, TIMER_COMBAT_OOC);
        AddCustomAction(HYDROSS_SPAWN_ELEMENTAL, 20000u, [&]() { HandleSummonElemental(); }, TIMER_COMBAT_OOC);
    }

    instance_serpentshrine_cavern* m_instance;

    uint8 m_uiMarkCount;
    bool m_corruptedForm;
    bool m_triggerSwitch;

    void Reset() override
    {
        CombatAI::Reset();

        m_corruptedForm = false;
        m_triggerSwitch = false;

        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

        m_creature->SetSpellList(SPELL_LIST_CLEAN_INITIAL);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        if (m_corruptedForm)
            DoBroadcastText(urand(0, 1) ? SAY_CORRUPT_SLAY1 : SAY_CORRUPT_SLAY2, m_creature);
        else
            DoBroadcastText(urand(0, 1) ? SAY_CLEAN_SLAY1 : SAY_CLEAN_SLAY2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(m_corruptedForm ? SAY_CORRUPT_DEATH : SAY_CLEAN_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HYDROSS_EVENT, FAIL);
    }

    // Adds summon during phase switch
    void DoSpawnAdds()
    {
        if (m_corruptedForm)
        {
            for (uint32 spellId : purifiedSpawns)
                m_creature->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
        }
        else
            for (uint32 spellId : corruptedSpawns)
                m_creature->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
    }

    // Wrapper to handle the blue beams animation
    void HandleBeamHelpers(bool reset)
    {
        if (!m_instance)
            return;

        GuidList lBeamHelpersGuid;
        m_instance->GetBeamHelpersGUIDList(lBeamHelpersGuid);

        for (GuidList::const_iterator itr = lBeamHelpersGuid.begin(); itr != lBeamHelpersGuid.end(); ++itr)
        {
            if (Creature* beam = m_creature->GetMap()->GetCreature(*itr))
            {
                if (reset)
                    beam->InterruptNonMeleeSpells(false);
                else
                    beam->CastSpell(nullptr, SPELL_BLUE_BEAM, TRIGGERED_NONE);
            }
        }
    }

    bool CheckTransition() // checks whether hydross is within initial circle
    {
        Position pos;
        m_creature->GetCombatStartPosition(pos);
        return m_creature->IsWithinDist2d(pos.GetPositionX(), pos.GetPositionY(), SWITCH_RADIUS);
    }

    void HandleSummonElemental()
    {
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEMENTAL);
        ResetTimer(HYDROSS_SPAWN_ELEMENTAL, 20000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (bool(miscValue) == m_corruptedForm)
                m_triggerSwitch = true;
        }
    }

    void HandleSwitchToClean()
    {
        DoBroadcastText(SAY_SWITCH_TO_CLEAN, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION_SD);
        m_uiMarkCount = 0;

        HandleBeamHelpers(false);
        DoResetThreat();
        DoSpawnAdds();

        m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_FROST);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

        m_corruptedForm = false;

        for (uint32 spellId : aMarkHydross)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
            m_creature->RemoveSpellCooldown(*spellInfo, false);
        }

        m_creature->SetSpellList(SPELL_LIST_CLEAN_REPEAT);
    }

    void HandleSwitchToCorruption()
    {
        if (DoCastSpellIfCan(nullptr, SPELL_CORRUPTION_SD) == CAST_OK)
        {
            DoBroadcastText(SAY_SWITCH_TO_CORRUPT, m_creature);
            m_uiMarkCount = 0;

            HandleBeamHelpers(true);
            DoResetThreat();
            DoSpawnAdds();

            m_creature->SetMeleeDamageSchool(SPELL_SCHOOL_NATURE);
            m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
            m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);

            m_corruptedForm = true;

            for (uint32 spellId : aMarkCorruption)
            {
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                m_creature->RemoveSpellCooldown(*spellInfo, false);
            }

            m_creature->SetSpellList(SPELL_LIST_CORRUPT);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == HYDROSS_SWITCH) // done here so that it does not overlap with casting or smth else
        {
            if (!m_triggerSwitch)
                return;

            m_triggerSwitch = false;
            if (m_corruptedForm)
                HandleSwitchToClean();
            else
                HandleSwitchToCorruption();
        }
    }
};

struct npc_spawn_of_hydrossAI : public CombatAI
{
    npc_spawn_of_hydrossAI(Creature* creature) : CombatAI(creature, 0)
    {
        m_creature->CastSpell(nullptr, SPELL_ELEMENTAL_SPAWNIN, TRIGGERED_OLD_TRIGGERED);
        SetReactState(REACT_PASSIVE);
        AddCustomAction(0, 3000u, [&]()
        {
            m_creature->SetInCombatWithZone();
            SetReactState(REACT_AGGRESSIVE);
        });
    }
};

// 37935 - Cleansing Field
struct CleansingField : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetTarget()->GetEntry() == NPC_HYDROSS_THE_UNSTABLE && aura->GetTarget()->AI())
            aura->GetTarget()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, aura->GetTarget(), aura->GetTarget(), apply);
    }
};

void AddSC_boss_hydross_the_unstable()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hydross_the_unstable";
    pNewScript->GetAI = &GetNewAIInstance<boss_hydross_the_unstableAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spawn_of_hydross";
    pNewScript->GetAI = &GetNewAIInstance<npc_spawn_of_hydrossAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<CleansingField>("spell_cleansing_field");
}
