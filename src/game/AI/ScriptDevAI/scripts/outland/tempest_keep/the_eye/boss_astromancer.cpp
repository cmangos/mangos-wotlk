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
SDName: Boss_Astromancer
SD%Complete: 90
SDComment: Check if the split phase has some spells involved
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "the_eye.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                           = -1550007,
    SAY_SUMMON1                         = -1550008,
    SAY_SUMMON2                         = -1550009,
    SAY_KILL1                           = -1550010,
    SAY_KILL2                           = -1550011,
    SAY_KILL3                           = -1550012,
    SAY_DEATH                           = -1550013,
    SAY_VOIDA                           = -1550014,
    SAY_VOIDB                           = -1550015,

    SPELL_ARCANE_MISSILES               = 33031,
    SPELL_WRATH_OF_THE_ASTROMANCER      = 42783,
    SPELL_WRATH_OF_THE_ASTROMANCER_DOT  = 42784,            // AoE damage on bomb application.
    SPELL_BLINDING_LIGHT                = 33009,
    SPELL_PSYHIC_SCREAM                 = 34322,
    SPELL_SOLARIAN_TRANSFORM            = 39117,
    SPELL_VOID_BOLT                     = 39329,
    SPELL_MARK_OF_SOLARIAN              = 33023,            // acts as an enrage spell
    // SPELL_ROTATE_ASTROMANCER         = 33283,            // purpose unk

    // prenerf wrath
    SPELL_WOTA_MAIN_CAST                = 33040,
    SPELL_WOTA_ARCANE_DEBUFF            = 33044,
    SPELL_WOTA_DOT                      = 33045,
    SPELL_WOTA_SEARCH                   = 33048,
    SPELL_WOTA_JUMP                     = 33049,
    SPELL_REMOVE_WOTA_OLD               = 33229,

    SPELL_ASTROMANCER_ADDS              = 33362,
    SPELL_ASTROMANCER_PRIEST            = 33367,

    // summoned creatures
    NPC_SOLARIUM_AGENT                  = 18925,
    NPC_SOLARIUM_PRIEST                 = 18806,
    NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT  = 18928,
    // NPC_ASTROMANCER_TRIGGER           = 18932,           // purpose unk

    // summoned spells
    SPELL_SPOTLIGHT                     = 25824,            // visual aura on the spotlights

    SPELL_SOLARIUM_GREAT_HEAL           = 33387,
    SPELL_SOLARIUM_HOLY_SMITE           = 25054,
    SPELL_SOLARIUM_ARCANE_TORRENT       = 33390,

    // TODO: Unused solarian adds summon spells - 33362,33367
    // TODO: Unused spell 33365 at start of split

    WV_ARMOR                            = 31000,            // ToDo: this value need to be checked

    MAX_SPOTLIGHTS                      = 3,
    MAX_AGENTS                          = 4,
};

// Spells used to summon the Spotlights on 2.4.3 - Astromancer Split
// The boss had to choose 2 large radius split spells and 1 small radius split
// Large radius spotlight: 33189,33281,33282,33347,33348,33349,33350,33351
// Small radius spotlight: 33352,33353,33354,33355

static const float fRoomCenter[4] = {432.909f, -373.424f, 17.9608f, 1.06421f};
static const float fSpotlightRadius[2] = {13.0f, 25.0f};

static const uint32 largeRadiusSpells[8] = { 33189, 33281, 33282, 33347, 33348, 33349, 33350, 33351 };
static const uint32 smallRadiusSpells[4] = { 33352, 33353, 33354, 33355 };

enum Phases
{
    PHASE_NORMAL        = 1,
    PHASE_SPLIT         = 2,
    PHASE_VOID          = 3,
};

enum SolarianActions
{
    SOLARIAN_PHASE_2,
    SOLARIAN_SPLIT_PHASE,
    SOLARIAN_WRATH_OF_THE_ASTROMANCER,
    SOLARIAN_ARCANE_MISSILES,
    SOLARIAN_BLINDING_LIGHT,
    SOLARIAN_FEAR,
    SOLARIAN_VOID_BOLT,
    SOLARIAN_ACTION_MAX,
    SOLARIAN_PHASE_2_DELAY,
    SOLARIAN_SPLIT_PHASE_DELAY,
    SOLARIAN_SPLIT_AGENTS,
    SOLARIAN_SPLIT_PRIESTS,
};

struct boss_high_astromancer_solarianAI : public CombatAI
{
    boss_high_astromancer_solarianAI(Creature* creature) : CombatAI(creature, SOLARIAN_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(SOLARIAN_PHASE_2, true);
        AddCombatAction(SOLARIAN_SPLIT_PHASE, 50000u);
        AddCombatAction(SOLARIAN_WRATH_OF_THE_ASTROMANCER, 25000, 30000);
        AddCombatAction(SOLARIAN_ARCANE_MISSILES, 0u);
        AddCombatAction(SOLARIAN_BLINDING_LIGHT, 30000u);
        AddCombatAction(SOLARIAN_VOID_BOLT, true);
        AddCombatAction(SOLARIAN_FEAR, true);
        AddCustomAction(SOLARIAN_PHASE_2_DELAY, true, [&]() { HandlePhase2Delay(); });
        AddCustomAction(SOLARIAN_SPLIT_PHASE_DELAY, true, [&]() { HandleSplitPhaseDelay(); });
        AddCustomAction(SOLARIAN_SPLIT_AGENTS, true, [&]() { HandleSplitAgents(); });
        AddCustomAction(SOLARIAN_SPLIT_PRIESTS, true, [&]() { HandleSplitPriests(); });
        m_uiDefaultArmor = m_creature->GetArmor();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiDefaultArmor;

    Phases m_Phase;

    GuidVector m_vSpotLightsGuidVector;

    void Reset() override
    {
        CombatAI::Reset();

        m_Phase = PHASE_NORMAL;

        // The vector will store the summoned spotlights
        m_vSpotLightsGuidVector.reserve(MAX_SPOTLIGHTS);

        m_creature->SetArmor(m_uiDefaultArmor);
        if (m_creature->GetVisibility() != VISIBILITY_ON)
            m_creature->SetVisibility(VISIBILITY_ON);

        SetCombatMovement(true);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL1, m_creature); break;
            case 1: DoScriptText(SAY_KILL2, m_creature); break;
            case 2: DoScriptText(SAY_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SOLARIAN, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SOLARIAN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SOLARIAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT:
                // Note: this should be moved to database
                pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSummoned->CastSpell(pSummoned, SPELL_SPOTLIGHT, TRIGGERED_NONE);
                m_vSpotLightsGuidVector.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_SOLARIUM_AGENT:
            case NPC_SOLARIUM_PRIEST:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    pSummoned->AI()->AttackStart(target);
                    pSummoned->AddThreat(target, 100000.f); // explicitly used in script without spell
                }
                break;
        }
    }

    void DoSummonSpotlights()
    {
        // none of these send packets
        m_creature->CastSpell(nullptr, smallRadiusSpells[urand(0, 3)], TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, largeRadiusSpells[urand(0, 3)], TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, largeRadiusSpells[urand(4, 7)], TRIGGERED_OLD_TRIGGERED);
    }

    void HandlePhase2Delay()
    {
        DoScriptText(SAY_VOIDB, m_creature);

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);

        DisableCombatAction(SOLARIAN_WRATH_OF_THE_ASTROMANCER);
        DisableCombatAction(SOLARIAN_ARCANE_MISSILES);
        DisableCombatAction(SOLARIAN_BLINDING_LIGHT);
        ResetCombatAction(SOLARIAN_FEAR, 20000);
        ResetCombatAction(SOLARIAN_VOID_BOLT, 10000);
    }

    void HandleSplitPhaseDelay()
    {
        // select two different numbers between 0 and 7 so we will get different spawn points for the spotlights
        uint8 uiPos1 = urand(0, 7);
        uint8 uiPos2 = (uiPos1 + urand(1, 7)) % 8;

        // summon 3 spotlights
        m_vSpotLightsGuidVector.clear();
        DoSummonSpotlights();

        m_creature->SetVisibility(VISIBILITY_OFF);

        DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);
        ResetTimer(SOLARIAN_SPLIT_AGENTS, 5000);
    }

    void HandleSplitAgents()
    {
        for (uint8 i = 0; i < MAX_SPOTLIGHTS; ++i)
            if (Creature* spotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[i]))
                spotlight->CastSpell(nullptr, SPELL_ASTROMANCER_ADDS, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());

        ResetTimer(SOLARIAN_SPLIT_PRIESTS, 16000);
    }

    void HandleSplitPriests()
    {
        m_Phase = PHASE_NORMAL;
        // Randomize the portals
        std::shuffle(m_vSpotLightsGuidVector.begin(), m_vSpotLightsGuidVector.end(), *GetRandomGenerator());
        // Summon 2 priests
        for (uint8 i = 0; i < 2; ++i)
            if (Creature* spotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[i]))
                spotlight->CastSpell(nullptr, SPELL_ASTROMANCER_PRIEST, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
        // Teleport the boss at the last portal
        if (Creature* spotlight = m_creature->GetMap()->GetCreature(m_vSpotLightsGuidVector[2]))
            m_creature->NearTeleportTo(spotlight->GetPositionX(), spotlight->GetPositionY(), spotlight->GetPositionZ(), spotlight->GetOrientation(), true);

        SetCombatScriptStatus(false);
        SetCombatMovement(true, true);

        // Set as visible and reset spells timers
        m_creature->SetVisibility(VISIBILITY_ON);
        ResetCombatAction(SOLARIAN_ARCANE_MISSILES, 0);
        ResetCombatAction(SOLARIAN_BLINDING_LIGHT, 30000u);
        ResetCombatAction(SOLARIAN_WRATH_OF_THE_ASTROMANCER, urand(20000, 25000));
        ResetCombatAction(SOLARIAN_SPLIT_PHASE, 70000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SOLARIAN_PHASE_2:
            {
                if (m_creature->GetHealthPercent() < 20.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SOLARIAN_TRANSFORM) == CAST_OK)
                    {
                        DoScriptText(SAY_VOIDA, m_creature);
                        ResetTimer(SOLARIAN_PHASE_2_DELAY, 2000);

                        m_creature->SetArmor(WV_ARMOR);
                        m_Phase = PHASE_VOID;

                        if (m_creature->GetVisibility() != VISIBILITY_ON)
                            m_creature->SetVisibility(VISIBILITY_ON);

                        // Stop the combat for a small delay
                        SetCombatMovement(false);
                        SetCombatScriptStatus(true);
                        SetMeleeEnabled(false);
                        SetActionReadyStatus(action, false);
                        DisableCombatAction(SOLARIAN_SPLIT_PHASE);
                        DisableCombatAction(SOLARIAN_ARCANE_MISSILES);
                    }
                }
                break;
            }
            case SOLARIAN_SPLIT_PHASE:
            {
                m_Phase = PHASE_SPLIT;

                // After these 50 seconds she portals to the middle of the room and disappears, leaving 3 light portals behind.
                // ToDo: check if there are some spells involved in this event!
                SetCombatMovement(false);
                SetMeleeEnabled(false);
                SetCombatScriptStatus(true);
                m_creature->NearTeleportTo(fRoomCenter[0], fRoomCenter[1], fRoomCenter[2], fRoomCenter[3], true);
                DisableCombatAction(action);
                ResetTimer(SOLARIAN_SPLIT_PHASE_DELAY, 1000);
                break;
            }
            case SOLARIAN_WRATH_OF_THE_ASTROMANCER:
            {
#ifdef PRENERF_2_0_3
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_WOTA_MAIN_CAST, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_WOTA_MAIN_CAST) == CAST_OK)
                        ResetCombatAction(action, urand(25000, 30000));
#else
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_WRATH_OF_THE_ASTROMANCER, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_WRATH_OF_THE_ASTROMANCER) == CAST_OK)
                        ResetCombatAction(action, urand(12000, 18000));
#endif
                break;
            }
            case SOLARIAN_ARCANE_MISSILES:
            {
                // Solarian casts Arcane Missiles on on random targets in the raid.
                std::vector<Unit*> units;
                m_creature->SelectAttackingTargets(units, ATTACKING_TARGET_ALL_SUITABLE, 0, SPELL_ARCANE_MISSILES, SELECT_FLAG_PLAYER);
                if (units.size() > 0)
                {
                    std::shuffle(units.begin(), units.end(), *GetRandomGenerator());
                    for (Unit* unit : units)
                    {
                        if (!unit->HasAura(SPELL_WOTA_DOT))
                        {
                            if (DoCastSpellIfCan(unit, SPELL_ARCANE_MISSILES) == CAST_OK)
                            {
                                ResetCombatAction(action, urand(3000, 3500));
                                break;
                            }
                        }
                    }
                }
                break;
            }
            case SOLARIAN_BLINDING_LIGHT:
            {
#ifdef PRENERF_2_0_3
                // She casts this spell every 45 seconds. It is a kind of Moonfire spell, which she strikes down on the whole raid simultaneously. It hits everyone in the raid for 2280 to 2520 arcane damage.
                if (DoCastSpellIfCan(nullptr, SPELL_BLINDING_LIGHT) == CAST_OK && DoCastSpellIfCan(nullptr, SPELL_MARK_OF_SOLARIAN) == CAST_OK)
#else
                if (DoCastSpellIfCan(nullptr, SPELL_BLINDING_LIGHT) == CAST_OK)
#endif
                    ResetCombatAction(action, 30000);
                break;
            }
            case SOLARIAN_FEAR:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_PSYHIC_SCREAM) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            }
            case SOLARIAN_VOID_BOLT:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_VOID_BOLT) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            }
        }
    }
};

enum SolarianPriestActions
{
    SOLARIAN_PRIEST_HEAL,
    SOLARIAN_PRIEST_ARCANE_TORRENT,
    SOLARIAN_PRIEST_HOLY_SMITE,
    SOLARIAN_PRIEST_ACTION_MAX,
};

struct mob_solarium_priestAI : public CombatAI
{
    mob_solarium_priestAI(Creature* creature) : CombatAI(creature, SOLARIAN_PRIEST_ACTION_MAX)
    {
        AddCombatAction(SOLARIAN_PRIEST_HEAL, 9000u);
        AddCombatAction(SOLARIAN_PRIEST_ARCANE_TORRENT, 15000u);
        AddCombatAction(SOLARIAN_PRIEST_HOLY_SMITE, 1u);
        SetRangedMode(true, 25.f, TYPE_PROXIMITY);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SOLARIAN_PRIEST_HEAL:
            {
                if (Unit* target = DoSelectLowestHpFriendly(50.0f, 1.0f, false, false))
                    if (DoCastSpellIfCan(target, SPELL_SOLARIUM_GREAT_HEAL) == CAST_OK)
                        ResetCombatAction(action, 9000);
                break;
            }
            case SOLARIAN_PRIEST_ARCANE_TORRENT:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SOLARIUM_ARCANE_TORRENT) == CAST_OK)
                    ResetCombatAction(action, 13000);
                break;
            }
            case SOLARIAN_PRIEST_HOLY_SMITE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SOLARIUM_HOLY_SMITE) == CAST_OK)
                        ResetCombatAction(action, 4000);
                break;
            }
        }
    }
};
struct WrathOfTheAstromancer : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                aura->GetTarget()->CastSpell(aura->GetTarget(), aura->GetSpellProto()->CalculateSimpleValue(SpellEffectIndex(aura->GetEffIndex() + 1)), TRIGGERED_OLD_TRIGGERED);
    }
};

struct WOTAOldMainCastAndJump : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        if (!target->IsInCombat())
            return;

        spell->GetCaster()->CastSpell(target, SPELL_WOTA_ARCANE_DEBUFF, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(target, SPELL_WOTA_DOT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct WOTAOldDOT : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_WOTA_SEARCH, TRIGGERED_OLD_TRIGGERED);
    }
};

struct WOTAOldSearch : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_CLOSEST);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        spell->GetCaster()->CastSpell(target, SPELL_WOTA_JUMP, TRIGGERED_OLD_TRIGGERED);
    }
};

struct WOTAOldRemove : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->RemoveAurasDueToSpell(SPELL_WOTA_ARCANE_DEBUFF);
        target->RemoveAurasDueToSpell(SPELL_WOTA_DOT);
    }
};

void AddSC_boss_high_astromancer_solarian()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_high_astromancer_solarian";
    pNewScript->GetAI = &GetNewAIInstance<boss_high_astromancer_solarianAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_solarium_priest";
    pNewScript->GetAI = &GetNewAIInstance<mob_solarium_priestAI>;
    pNewScript->RegisterSelf();

    RegisterAuraScript<WrathOfTheAstromancer>("spell_wrath_of_the_astromancer");
    RegisterSpellScript<WOTAOldMainCastAndJump>("spell_wota_main_cast_and_jump");
    RegisterAuraScript<WOTAOldDOT>("spell_wota_dot");
    RegisterSpellScript<WOTAOldSearch>("spell_wota_search");
    RegisterSpellScript<WOTAOldRemove>("spell_wota_remove");
}
