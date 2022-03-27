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
SDName: Boss_Netherspite
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    // netherspite spells
    SPELL_NETHERBURN            = 30522,
    SPELL_VOID_ZONE             = 37063,                // NOTE: all void zone spells have Consumption spell associated with them
    SPELL_NETHERBREATH          = 38523,
    SPELL_EMPOWERMENT           = 38549,
    SPELL_NETHER_INFUSION       = 38688,                // hard enrage spell
    SPELL_NETHERSPITE_ROAR      = 38684,                // on banish phase begin
    SPELL_SHADOWFORM            = 38542,                // banish visual spell
    SPELL_FACE_RANDOM_TARGET    = 38546,                // triggered by spell 38684 - currently not used
    SPELL_PORTAL_ATTUNEMENT     = 30425,
    SPELL_DUMMY_NUKE            = 21912,

    // void zone spells
    SPELL_CONSUMPTION           = 28865,                // Has serverside timer in TBC
    SPELL_CONSUMPTION_TIMER     = 28874,                // Cast in EAI onto void zones on spawn
    SPELL_ROOT                  = 18373,                // dummy root spell to prevent void zones from moving

    // ***** Netherspite portals spells ***** //
    // beam buffs
    SPELL_SERENITY_NS           = 30467,
    SPELL_SERENITY_PLR          = 30422,
    SPELL_DOMINANCE_NS          = 30468,
    SPELL_DOMINANCE_PLR         = 30423,
    SPELL_PERSEVERENCE_NS       = 30466,
    SPELL_PERSEVERENCE_PLR      = 30421,

    // beam debuffs (player with this aura cannot gain the same color buff)
    SPELL_EXHAUSTION_PER        = 38637,
    SPELL_EXHAUSTION_SER        = 38638,
    SPELL_EXHAUSTION_DOM        = 38639,

    // spells which hit players (used only for visual - as seen from spell description)
    SPELL_BEAM_PER              = 30400,
    SPELL_BEAM_SER              = 30401,
    SPELL_BEAM_DOM              = 30402,

    // spells which hit Netherspite
    SPELL_BEAM_GREEN            = 30464,
    SPELL_BEAM_BLUE             = 30463,
    SPELL_BEAM_RED              = 30465,

    // portal visual spells
    SPELL_GREEN_PORTAL          = 30490,
    SPELL_BLUE_PORTAL           = 30491,
    SPELL_RED_PORTAL            = 30487,

    // passive auras
    SPELL_PERSEVERANCE_PASSIVE  = 30396,
    SPELL_SERENITY_PASSIVE      = 30397,
    SPELL_DOMINANCE_PASSIVE     = 30398,
    // SPELL_CLEAR_NETHER_BEAM  = 37072,                // not clear how to use this

    // emotes
    EMOTE_PHASE_BEAM            = -1532089,
    EMOTE_PHASE_BANISH          = -1532090,

    // npcs
    NPC_PORTAL_GREEN            = 17367,
    NPC_PORTAL_BLUE             = 17368,
    NPC_PORTAL_RED              = 17369,
    NPC_VOID_ZONE               = 16697,

    MAX_PORTALS                 = 3,
};

// at first spawn portals got fixed coords, should be shuffled in subsequent beam phases
static const Position aPortalCoordinates[MAX_PORTALS] =
{
    { -11195.14f, -1616.375f, 278.3217f, 6.230825f},
    { -11108.13f, -1602.839f, 280.0323f, 3.717551f},
    { -11139.78f, -1681.278f, 278.3217f, 1.396263f},
};

enum NetherspitePhases
{
    BEAM_PHASE   = 0,
    BANISH_PHASE = 1,
};

static const uint32 auiPortals[MAX_PORTALS] =
{
    NPC_PORTAL_GREEN,
    NPC_PORTAL_BLUE,
    NPC_PORTAL_RED,
};

/*######
## boss_netherspite
######*/

enum NetherspiteActions
{
    NETHERSPITE_PHASE_CHANGE,
    NETHERSPITE_ENRAGE,
    NETHERSPITE_EMPOWERMENT,
    NETHERSPITE_VOID_ZONE,
    NETHERSPITE_FACE_RANDOM_TARGET,
    NETHERSPITE_NETHERBREATH,
    NETHERSPITE_DUMMY_NUKE,
    NETHERSPITE_ACTION_MAX,
};

// TODO: Should propably have 25yd aggro range?
struct boss_netherspiteAI : public CombatAI
{
    boss_netherspiteAI(Creature* creature) : CombatAI(creature, NETHERSPITE_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(NETHERSPITE_PHASE_CHANGE, uint32(MINUTE * IN_MILLISECONDS));
        AddCombatAction(NETHERSPITE_ENRAGE, uint32(9 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(NETHERSPITE_EMPOWERMENT, 10000u);
        AddCombatAction(NETHERSPITE_VOID_ZONE, 15000u);
        AddCombatAction(NETHERSPITE_NETHERBREATH, true);
        AddCombatAction(NETHERSPITE_FACE_RANDOM_TARGET, true);
        AddCombatAction(NETHERSPITE_DUMMY_NUKE, true);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float, float, float) -> bool
        {
            return GetCombatScriptStatus() && m_instance && m_instance->GetPlayerInMap(true, false) == nullptr;
        });
        Reset();
    }

    ScriptedInstance* m_instance;

    NetherspitePhases m_uiActivePhase;

    std::vector<uint32> m_vPortalEntryList;

    std::vector<ObjectGuid> m_vPortalGuidList;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiActivePhase       = BEAM_PHASE;

        SetCombatMovement(true);
        SetMeleeEnabled(true);

        // initialize the portal list
        m_vPortalEntryList.clear();
        m_vPortalEntryList.resize(MAX_PORTALS);

        for (uint8 i = 0; i < MAX_PORTALS; ++i)
            m_vPortalEntryList[i] = auiPortals[i];

        m_creature->FixateTarget(nullptr);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NETHERSPITE, IN_PROGRESS);

        DoSummonPortals();
        DoCastSpellIfCan(m_creature, SPELL_NETHERBURN);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NETHERSPITE, DONE);

        DespawnPortals();
    }

    void DespawnPortals()
    {
        for (ObjectGuid& portalGuid : m_vPortalGuidList)
            if (Creature* portal = m_creature->GetMap()->GetCreature(portalGuid))
                portal->ForcedDespawn();

        m_vPortalGuidList.clear();
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        if (m_instance)
            m_instance->SetData(TYPE_NETHERSPITE, FAIL);
        
        DespawnPortals();
    }

    void SwitchPhases()
    {
        if (m_uiActivePhase == BEAM_PHASE)
        {            
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOWFORM, CAST_TRIGGERED) == CAST_OK)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_EMPOWERMENT);
#ifdef PRENERF_2_0_3
#else
                m_creature->RemoveAurasDueToSpell(SPELL_NETHERBURN);
#endif
                DoCastSpellIfCan(m_creature, SPELL_NETHERSPITE_ROAR);

                SetCombatMovement(false);
                SetMeleeEnabled(false);

                m_uiActivePhase = BANISH_PHASE;
                DoScriptText(EMOTE_PHASE_BANISH, m_creature);

                DisableCombatAction(NETHERSPITE_EMPOWERMENT);
                DisableCombatAction(NETHERSPITE_VOID_ZONE);
                ResetCombatAction(NETHERSPITE_FACE_RANDOM_TARGET, 5000);
                ResetCombatAction(NETHERSPITE_NETHERBREATH, 5000);
                ResetCombatAction(NETHERSPITE_DUMMY_NUKE, 6000);
                ResetCombatAction(NETHERSPITE_PHASE_CHANGE, 30000);

                DespawnPortals();
            }
        }
        else
        {
#ifdef PRENERF_2_0_3
#else
            DoCastSpellIfCan(m_creature, SPELL_NETHERBURN, CAST_TRIGGERED);
#endif
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
            DoCastSpellIfCan(m_creature, SPELL_NETHERSPITE_ROAR);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            m_creature->FixateTarget(nullptr);

            if (m_creature->GetVictim())
            {
                m_creature->SetTarget(m_creature->GetVictim());
                DoStartMovement(m_creature->GetVictim());
            }

            m_uiActivePhase = BEAM_PHASE;
            DoScriptText(EMOTE_PHASE_BEAM, m_creature);

            DoSummonPortals();
            DisableCombatAction(NETHERSPITE_FACE_RANDOM_TARGET);
            DisableCombatAction(NETHERSPITE_NETHERBREATH);
            DisableCombatAction(NETHERSPITE_DUMMY_NUKE);
            ResetCombatAction(NETHERSPITE_EMPOWERMENT, 10000);
            ResetCombatAction(NETHERSPITE_VOID_ZONE, 15000);
            ResetCombatAction(NETHERSPITE_PHASE_CHANGE, MINUTE * IN_MILLISECONDS);
        }

        // reset threat every phase switch
        DoResetThreat();
    }

    void DoSummonPortals()
    {
        for (uint8 i = 0; i < MAX_PORTALS; ++i)
            m_creature->SummonCreature(m_vPortalEntryList[i], aPortalCoordinates[i].x, aPortalCoordinates[i].y, aPortalCoordinates[i].z, aPortalCoordinates[i].o, TEMPSPAWN_DEAD_DESPAWN, 0);

        // randomize the portals after the first summon
        std::shuffle(m_vPortalEntryList.begin(), m_vPortalEntryList.end(), *GetRandomGenerator());
    }

    void JustSummoned(Creature* summoned) override
    {
        m_vPortalGuidList.push_back(summoned->GetObjectGuid());

        switch (summoned->GetEntry())
        {
            case NPC_PORTAL_RED:
                summoned->CastSpell(summoned, SPELL_RED_PORTAL, TRIGGERED_NONE);
                break;
            case NPC_PORTAL_GREEN:
                summoned->CastSpell(summoned, SPELL_GREEN_PORTAL, TRIGGERED_NONE);
                break;
            case NPC_PORTAL_BLUE:
                summoned->CastSpell(summoned, SPELL_BLUE_PORTAL, TRIGGERED_NONE);
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NETHERSPITE_PHASE_CHANGE:
            {
                SwitchPhases();
                break;
            }
            case NETHERSPITE_ENRAGE:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_NETHER_INFUSION) == CAST_OK)
                    DisableCombatAction(action);
                break;
            }
            case NETHERSPITE_EMPOWERMENT:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EMPOWERMENT) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_PORTAL_ATTUNEMENT, CAST_TRIGGERED);
                    DisableCombatAction(action);
                }
                break;
            }
            case NETHERSPITE_VOID_ZONE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_VOID_ZONE) == CAST_OK)
                        ResetCombatAction(action, 15000);
                break;
            }
            case NETHERSPITE_FACE_RANDOM_TARGET:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FACE_RANDOM_TARGET) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            }
            case NETHERSPITE_NETHERBREATH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_NETHERBREATH, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_NETHERBREATH) == CAST_OK)
                        ResetCombatAction(action, 6000);
                break;
            }
            case NETHERSPITE_DUMMY_NUKE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DUMMY_NUKE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_DUMMY_NUKE) == CAST_OK)
                        ResetCombatAction(action, 2000);
                break;
            }
        }
    }
};

/*######
## npc_netherspite_portal
######*/

struct npc_netherspite_portalAI : public Scripted_NoMovementAI
{
    npc_netherspite_portalAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiOrientationTimer = 0;
        m_creature->GetCombatManager().SetLeashingDisable(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiOrientationTimer;

    ObjectGuid m_uiCurrentBeamTarget;

    void Reset() override
    {
        m_uiCurrentBeamTarget = ObjectGuid();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (pInvoker->GetEntry() != NPC_NETHERSPITE)
                return;

            // update orientation every second to focus on Netherspite
            m_uiOrientationTimer = 1000;
            float angle = m_creature->GetAngle(pInvoker);
            m_creature->SetFacingTo(angle);
            m_creature->SetOrientation(angle);

            switch (m_creature->GetEntry())
            {
                case NPC_PORTAL_GREEN:
                    DoCastSpellIfCan(nullptr, SPELL_SERENITY_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                    break;
                case NPC_PORTAL_BLUE:
                    DoCastSpellIfCan(nullptr, SPELL_DOMINANCE_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                    break;
                case NPC_PORTAL_RED:
                    DoCastSpellIfCan(nullptr, SPELL_PERSEVERANCE_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                    break;
            }
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* pSpell) override
    {
        if (target->GetObjectGuid() != m_uiCurrentBeamTarget)
        {
            switch (pSpell->Id)
            {
                case SPELL_BEAM_PER:
                    if (Creature* pNetherspite = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHERSPITE))
                        pNetherspite->FixateTarget(target);
                case SPELL_BEAM_SER:
                case SPELL_BEAM_DOM:
                    m_uiCurrentBeamTarget = target->GetObjectGuid();
                    break;
                case SPELL_BEAM_RED:
                case SPELL_BEAM_BLUE:
                case SPELL_BEAM_GREEN:
                    m_uiCurrentBeamTarget = target->GetObjectGuid();
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiOrientationTimer)
        {
            if (m_uiOrientationTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pNetherspite = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHERSPITE))
                    {
                        float angle = m_creature->GetAngle(pNetherspite);
                        m_creature->SetFacingTo(angle);
                        m_creature->SetOrientation(angle);
                    }
                }
                m_uiOrientationTimer = 1000;
            }
            else
                m_uiOrientationTimer -= uiDiff;
        }
    }
};

struct PortalAttunement : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        std::set<uint32> ids = { NPC_PORTAL_RED, NPC_PORTAL_GREEN, NPC_PORTAL_BLUE };

        Unit* target = spell->GetUnitTarget();
        if (!target || ids.find(target->GetEntry()) == ids.end())
            return;

        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), target);
    }
};

struct NetherBeam : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetFilteringScheme(EFFECT_INDEX_0, false, SCHEME_CLOSEST);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        Unit* caster = spell->GetCaster();

        // The player and boss spells are different
        if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        {
            switch (caster->GetEntry())
            {
                case 17367:
                    if (unitTarget->HasAura(38638))
                        return;

                    unitTarget->CastSpell(unitTarget, 30422, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30401))
                        caster->CastSpell(unitTarget, 30401, TRIGGERED_NONE);
                    break;
                case 17368:
                    if (unitTarget->HasAura(38639))
                        return;

                    unitTarget->CastSpell(unitTarget, 30423, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30402))
                        caster->CastSpell(unitTarget, 30402, TRIGGERED_NONE);
                    break;
                case 17369:
                    if (unitTarget->HasAura(38637))
                        return;

                    unitTarget->CastSpell(unitTarget, 30421, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30400))
                        caster->CastSpell(unitTarget, 30400, TRIGGERED_NONE);
                    break;
            }
        }
        // target boss
        else if (unitTarget->GetEntry() == 15689)
        {
            switch (caster->GetEntry())
            {
                case 17367:
                    unitTarget->CastSpell(unitTarget, 30467, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30464))
                        caster->CastSpell(unitTarget, 30464, TRIGGERED_NONE);
                    break;
                case 17368:
                    unitTarget->CastSpell(unitTarget, 30468, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30463))
                        caster->CastSpell(unitTarget, 30463, TRIGGERED_NONE);
                    break;
                case 17369:
                    unitTarget->CastSpell(unitTarget, 30466, TRIGGERED_OLD_TRIGGERED);
                    if (!unitTarget->HasAura(30465))
                        caster->CastSpell(unitTarget, 30465, TRIGGERED_NONE);
                    break;
            }
        }
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetTypeId() == TYPEID_UNIT && target->GetEntry() != 15689)
            return false;
        switch (spell->GetTriggeredByAuraSpellInfo()->Id)
        {
            case 30396: // Perseverance passive
                if (target->HasAura(38637))
                    return false;
                break;
            case 30397: // Serenity passive
                if (target->HasAura(38638))
                    return false;
                break;
            case 30398: // Dominance passive
                if (target->HasAura(38639))
                    return false;
                break;
        }
        if (!target->IsAlive())
            return false;
        return true;
    }
};

struct FaceRandomTarget : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Spell::TargetList const& list = spell->GetTargetList();
        if (!list.empty())
        {
            auto itr = list.begin();
            std::advance(itr, urand(0, list.size() - 1));
            Unit* target = spell->GetCaster()->GetMap()->GetPlayer((*itr).targetGUID);
            if (target)
                spell->GetCaster()->FixateTarget(target);
        }
    }
};

void AddSC_boss_netherspite()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_netherspite";
    pNewScript->GetAI = &GetNewAIInstance<boss_netherspiteAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_netherspite_portal";
    pNewScript->GetAI = &GetNewAIInstance<npc_netherspite_portalAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PortalAttunement>("spell_portal_attunement");
    RegisterSpellScript<NetherBeam>("spell_nether_beam");
    RegisterSpellScript<FaceRandomTarget>("spell_face_random_target");
}
