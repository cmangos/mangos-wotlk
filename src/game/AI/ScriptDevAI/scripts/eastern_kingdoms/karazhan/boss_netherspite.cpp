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

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"

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

struct SpawnLocation
{
    float fX, fY, fZ, fO;
};

// at first spawn portals got fixed coords, should be shuffled in subsequent beam phases
static const SpawnLocation aPortalCoordinates[MAX_PORTALS] =
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

// TODO: Should propably have 25yd aggro range?
struct boss_netherspiteAI : public ScriptedAI
{
    boss_netherspiteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    NetherspitePhases m_uiActivePhase;

    uint32 m_uiEnrageTimer;
    uint32 m_uiVoidZoneTimer;
    uint32 m_uiPhaseSwitchTimer;
    uint32 m_uiNetherbreathTimer;
    uint32 m_uiEmpowermentTimer;

    std::vector<uint32> m_vPortalEntryList;

    std::vector<ObjectGuid> m_vPortalGuidList;

    void Reset() override
    {
        m_uiActivePhase       = BEAM_PHASE;

        m_uiEmpowermentTimer  = 10000;
        m_uiEnrageTimer       = 9 * MINUTE * IN_MILLISECONDS;
        m_uiVoidZoneTimer     = 15000;
        m_uiPhaseSwitchTimer  = MINUTE * IN_MILLISECONDS;

        SetCombatMovement(true);

        // initialize the portal list
        m_vPortalEntryList.clear();
        m_vPortalEntryList.resize(MAX_PORTALS);

        for (uint8 i = 0; i < MAX_PORTALS; ++i)
            m_vPortalEntryList[i] = auiPortals[i];

        m_creature->FixateTarget(nullptr);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, IN_PROGRESS);

        DoSummonPortals();
        DoCastSpellIfCan(m_creature, SPELL_NETHERBURN);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, DONE);

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

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, FAIL);
        
        DespawnPortals();
    }

    void SwitchPhases()
    {
        if (m_uiActivePhase == BEAM_PHASE)
        {            
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOWFORM, CAST_TRIGGERED) == CAST_OK)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_EMPOWERMENT);
                m_creature->RemoveAurasDueToSpell(SPELL_NETHERBURN);
                DoCastSpellIfCan(m_creature, SPELL_NETHERSPITE_ROAR);

                SetCombatMovement(false);
                SetCombatScriptStatus(true);
                SetMeleeEnabled(false);
                m_creature->SetTarget(nullptr);

                m_uiActivePhase = BANISH_PHASE;
                DoScriptText(EMOTE_PHASE_BANISH, m_creature);

                m_uiNetherbreathTimer = 2000;
                m_uiPhaseSwitchTimer  = 30000;

                DespawnPortals();
            }
        }
        else
        {
            DoCastSpellIfCan(m_creature, SPELL_NETHERBURN, CAST_TRIGGERED);
            m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
            DoCastSpellIfCan(m_creature, SPELL_NETHERSPITE_ROAR);
            SetCombatMovement(true);
            SetCombatScriptStatus(false);
            SetMeleeEnabled(true);

            if (m_creature->getVictim())
            {
                m_creature->SetTarget(m_creature->getVictim());
                DoStartMovement(m_creature->getVictim());
            }

            m_uiActivePhase = BEAM_PHASE;
            DoScriptText(EMOTE_PHASE_BEAM, m_creature);

            DoSummonPortals();
            m_uiEmpowermentTimer  = 10000;
            m_uiPhaseSwitchTimer  = MINUTE * IN_MILLISECONDS;
        }

        // reset threat every phase switch
        DoResetThreat();
    }

    void DoSummonPortals()
    {
        for (uint8 i = 0; i < MAX_PORTALS; ++i)
            m_creature->SummonCreature(m_vPortalEntryList[i], aPortalCoordinates[i].fX, aPortalCoordinates[i].fY, aPortalCoordinates[i].fZ, aPortalCoordinates[i].fO, TEMPSPAWN_DEAD_DESPAWN, 0);

        // randomize the portals after the first summon
        std::random_shuffle(m_vPortalEntryList.begin(), m_vPortalEntryList.end());
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_vPortalGuidList.push_back(pSummoned->GetObjectGuid());

        switch (pSummoned->GetEntry())
        {
            case NPC_PORTAL_RED:
                pSummoned->CastSpell(pSummoned, SPELL_RED_PORTAL, TRIGGERED_NONE);
                break;
            case NPC_PORTAL_GREEN:
                pSummoned->CastSpell(pSummoned, SPELL_GREEN_PORTAL, TRIGGERED_NONE);
                break;
            case NPC_PORTAL_BLUE:
                pSummoned->CastSpell(pSummoned, SPELL_BLUE_PORTAL, TRIGGERED_NONE);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget())
            return;

        if (m_uiPhaseSwitchTimer <= uiDiff)
            SwitchPhases();
        else
            m_uiPhaseSwitchTimer -= uiDiff;

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_NETHER_INFUSION) == CAST_OK)
                    m_uiEnrageTimer = 0;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        if (m_uiActivePhase == BEAM_PHASE)
        {
            if (m_uiVoidZoneTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_VOID_ZONE) == CAST_OK)
                        m_uiVoidZoneTimer = 15000;
                }
            }
            else
                m_uiVoidZoneTimer -= uiDiff;

            if (m_uiEmpowermentTimer)
            {
                if (m_uiEmpowermentTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_EMPOWERMENT) == CAST_OK)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_PORTAL_ATTUNEMENT, CAST_TRIGGERED);
                        m_uiEmpowermentTimer = 0;
                    }
                }
                else
                    m_uiEmpowermentTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiNetherbreathTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_NETHERBREATH, SELECT_FLAG_PLAYER))
                {
                    m_creature->SetInFront(pTarget);
                    if (DoCastSpellIfCan(pTarget, SPELL_NETHERBREATH) == CAST_OK)
                        m_uiNetherbreathTimer = urand(4000, 5000);
                }
            }
            else
                m_uiNetherbreathTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_boss_netherspite(Creature* pCreature)
{
    return new boss_netherspiteAI(pCreature);
}

/*######
## npc_netherspite_portal
######*/

struct npc_netherspite_portalAI : public Scripted_NoMovementAI
{
    npc_netherspite_portalAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiOrientationTimer = 0;
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

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetObjectGuid() != m_uiCurrentBeamTarget)
        {
            switch (pSpell->Id)
            {
                case SPELL_BEAM_PER:
                    if (Creature* pNetherspite = m_pInstance->GetSingleCreatureFromStorage(NPC_NETHERSPITE))
                        pNetherspite->FixateTarget(pTarget);
                case SPELL_BEAM_SER:
                case SPELL_BEAM_DOM:
                    m_uiCurrentBeamTarget = pTarget->GetObjectGuid();
                    break;
                case SPELL_BEAM_RED:
                case SPELL_BEAM_BLUE:
                case SPELL_BEAM_GREEN:
                    m_uiCurrentBeamTarget = pTarget->GetObjectGuid();
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

UnitAI* GetAI_npc_netherspite_portal(Creature* pCreature)
{
    return new npc_netherspite_portalAI(pCreature);
}

bool EffectScriptEffectCreature_spell_portal_attunement(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_PORTAL_ATTUNEMENT && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_PORTAL_RED || pCreatureTarget->GetEntry() == NPC_PORTAL_GREEN || pCreatureTarget->GetEntry() == NPC_PORTAL_BLUE)
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);

        return true;
    }

    return false;
}

void AddSC_boss_netherspite()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_netherspite";
    pNewScript->GetAI = &GetAI_boss_netherspite;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_netherspite_portal";
    pNewScript->GetAI = &GetAI_npc_netherspite_portal;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_portal_attunement;
    pNewScript->RegisterSelf();
}
