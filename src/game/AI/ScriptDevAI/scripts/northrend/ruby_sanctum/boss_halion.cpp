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
SDName: boss_halion
SD%Complete: 50
SDComment: Phase 3 and related transition NYI; Shadow Orbs NYI; Meteor Strikes NYI; Heroic abilities NYI.
SDCategory: Ruby Sanctum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ruby_sanctum.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1724025,
    SAY_SLAY_1                  = -1724026,
    SOUND_SLAY_2                = 17502,                    // not sure if this one has a text
    SAY_DEATH                   = -1724027,
    SAY_BERSERK                 = -1724028,
    SAY_FIREBALL                = -1724029,
    SAY_SPHERES                 = -1724030,
    SAY_PHASE_2                 = -1724031,
    SAY_PHASE_3                 = -1724032,

    EMOTE_SPHERES               = -1724033,
    EMOTE_OUT_OF_TWILLIGHT      = -1724034,
    EMOTE_OUT_OF_PHYSICAL       = -1724035,
    EMOTE_INTO_TWILLIGHT        = -1724036,
    EMOTE_INTO_PHYSICAL         = -1724037,
    EMOTE_REGENERATE            = -1724038,

    // *** Spells ***
    // General
    SPELL_TWILIGHT_PRECISION    = 78243,
    SPELL_CLEAVE                = 74524,
    SPELL_TAIL_LASH             = 74531,
    SPELL_BERSERK               = 26662,
    //share damage spell: 74810 - serverside spell
    // Transitions
    SPELL_TWILIGHT_PHASING      = 74808,                    // Halion real - encounter phase 2 aura; applied at the start of phase 2
    SPELL_SUMMON_PORTAL         = 74809,                    // summon GO 202794; GO has serverside spell 75074
    SPELL_TWILIGHT_DIVISION     = 75063,                    // Halion twilight; applied at the start of encounter phase 3
    SPELL_TWILIGHT_MENDING      = 75509,                    // Halion healing spell when left unchecked

    // Player spells
    SPELL_TWILIGHT_REALM        = 74807,                    // player phasing aura; applies aura phase 32
    SPELL_LEAVE_TWILIGHT_REALM  = 74812,                    // handled by GO 202796; remove phasing aura

    // Real
    SPELL_FLAME_BREATH          = 74525,
    SPELL_METEOR_SUMMON         = 74637,                    // summons 40029
    SPELL_FIERY_COMBUSTION      = 74562,                    // curse - triggers 74567 on self (player); on dispell triggers 74607 and 74610

    // Twilight
    SPELL_DARK_BREATH           = 74806,
    SPELL_DUSK_SHROUD           = 75476,
    SPELL_SOUL_CONSUMPTION      = 74792,                    // curse - triggers 74795 on self (player); on dispell triggers 74799 and 74800

    // Corporeality
    SPELL_CORPOREALITY_EVEN     = 74826,                    // Deals & receives normal damage
    SPELL_CORPOREALITY_20I      = 74827,                    // Damage dealt increased by 10%  - Damage taken increased by 15%
    SPELL_CORPOREALITY_40I      = 74828,                    // Damage dealt increased by 30%  - Damage taken increased by 50%
    SPELL_CORPOREALITY_60I      = 74829,                    // Damage dealt increased by 60%  - Damage taken increased by 100%
    SPELL_CORPOREALITY_80I      = 74830,                    // Damage dealt increased by 100% - Damage taken increased by 200%
    SPELL_CORPOREALITY_100I     = 74831,                    // Damage dealt increased by 200% - Damage taken increased by 400%
    SPELL_CORPOREALITY_20D      = 74832,                    // Damage dealt reduced by 10%    - Damage taken reduced by 15%
    SPELL_CORPOREALITY_40D      = 74833,                    // Damage dealt reduced by 30%    - Damage taken reduced by 50%
    SPELL_CORPOREALITY_60D      = 74834,                    // Damage dealt reduced by 60%    - Damage taken reduced by 100%
    SPELL_CORPOREALITY_80D      = 74835,                    // Damage dealt reduced by 100%   - Damage taken reduced by 200%
    SPELL_CORPOREALITY_100D     = 74836,                    // Damage dealt reduced by 200%   - Damage taken reduced by 400%

    // *** Other spells ***
    // Combustion
    // SPELL_COMBUSTION_PERIODIC   = 74629,                 // cast by npc 40001 in creature_template_addon

    // Consumption
    // SPELL_CONSUMPTION_PERIODIC  = 74803,                 // cast by npc 40135 in creature_template_addon

    // Meteor
    SPELL_METEOR_VISUAL         = 74641,                    // cast by npc 40029 (all meteor spells)
    SPELL_METEOR_IMPACT         = 74648,                    // cast on visual aura expire
    SPELL_METEOR_FLAME          = 74713,
    SPELL_METEOR_FLAME2         = 74718,                    // cast by the secondary strike npcs
    SPELL_BIRTH                 = 40031,                    // cast by the meteor strike npcs

    // Cutter
    SPELL_TWILIGHT_PULSE        = 78861,                    // cast by each shadow orb individually
    SPELL_TWILIGHT_CUTTER       = 74768,                    // cast by shadow orb 1 and 3 to shadow orb 2 and 4
    SPELL_TRACK_ROTATION        = 74758,                    // cast by 40081 on 40091

    // Living Inferno
    SPELL_BLAZING_AURA          = 75885,                    // cast by 40681

    // Living Ember
    SPELL_AWAKEN_FLAMES         = 75889,                    // cast by 40683

    // Npcs
    NPC_METEOR_STRIKE_MAIN      = 40029,                    // summons the other meteor strikes using serverside spells like 74680, 74681, 74682, 74683

    NPC_METEOR_STRIKE_1         = 40041,                    // Npc 40029 summons the first 4 secondary meteor strike npcs, then each of them summons one 40055 npc using serverside spells 74687, 74688
    NPC_METEOR_STRIKE_2         = 40042,
    NPC_METEOR_STRIKE_3         = 40043,
    NPC_METEOR_STRIKE_4         = 40044,
    NPC_METEOR_STRIKE_FLAME     = 40055,                    // Each npc 40055 summons other 10 40055 npcs resulting in a total spawns of 40 40055 npcs.

    // Heroic npcs
    NPC_LIVING_INFERNO          = 40681,                    // summoned by 75879 (heroic version spell)
    NPC_LIVING_EMBER            = 40683,

    // *** Phases ***
    PHASE_PHISYCAL_REALM        = 1,
    PHASE_TWILIGHT_REALM        = 2,
    PHASE_BOTH_REALMS           = 3,
};

static const uint32 aShadowOrbs[4] = { NPC_SHADOW_ORB_1, NPC_SHADOW_ORB_2, NPC_SHADOW_ORB_3, NPC_SHADOW_ORB_4 };
static const uint32 aMeteorStrikes[4] = { NPC_METEOR_STRIKE_1, NPC_METEOR_STRIKE_2, NPC_METEOR_STRIKE_3, NPC_METEOR_STRIKE_4 };

static const float aRotationFocusPosition[4] = { 3184.448f, 530.314f, 73.0f, 4.586f };
static const float aOrbCarrierPosition1[3] = { 3153.75f, 533.1875f, 72.972f };

/*######
## boss_halion_real
######*/

struct boss_halion_realAI : public ScriptedAI
{
    boss_halion_realAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_ruby_sanctum*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint8 m_uiPhase;

    uint32 m_uiTailLashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiFieryCombustionTimer;
    uint32 m_uiMeteorTimer;
    uint32 m_uiFlameBreathTimer;

    void Reset() override
    {
        m_uiPhase                   = PHASE_PHISYCAL_REALM;

        m_uiTailLashTimer           = 10000;
        m_uiCleaveTimer             = urand(5000, 10000);
        m_uiFieryCombustionTimer    = 15000;
        m_uiMeteorTimer             = 20000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_HALION, IN_PROGRESS);
            m_pInstance->SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, m_creature->GetObjectGuid(), 1);

            // set berserk timer
            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, pController, 8 * MINUTE * IN_MILLISECONDS);
        }

        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_PRECISION, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            switch (urand(0, 1))
            {
                case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
                case 1: DoPlaySoundToSet(m_creature, SOUND_SLAY_2); break;
            }
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALION, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALION, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_METEOR_STRIKE_MAIN:
                // ToDo: summon the other meteor strikes around this one
                pSummoned->CastSpell(pSummoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_METEOR_VISUAL, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    // share damage with the twilight version
    void DamageTaken(Unit* dealer, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (m_creature->GetHealth() <= damage)
            damage = m_creature->GetHealth() - 1;

        if (m_pInstance)
        {
            if (Creature* halion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                Unit::DealDamage(dealer, halion, damage, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        // phase 3 switch
        if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_PHASING);
            DoScriptText(SAY_PHASE_3, m_creature);
            m_uiPhase = PHASE_BOTH_REALMS;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_BOTH_REALMS:
            // ToDo: handle corporeality
            // no break;
            case PHASE_PHISYCAL_REALM:

                if (m_uiTailLashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_LASH) == CAST_OK)
                        m_uiTailLashTimer = urand(15000, 25000);
                }
                else
                    m_uiTailLashTimer -= uiDiff;

                if (m_uiCleaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(10000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                if (m_uiFlameBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_BREATH) == CAST_OK)
                        m_uiFlameBreathTimer = urand(15000, 20000);
                }
                else
                    m_uiFlameBreathTimer -= uiDiff;

                if (m_uiFieryCombustionTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_FIERY_COMBUSTION, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_FIERY_COMBUSTION) == CAST_OK)
                            m_uiFieryCombustionTimer = 25000;
                    }
                }
                else
                    m_uiFieryCombustionTimer -= uiDiff;

                if (m_uiMeteorTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_METEOR_SUMMON) == CAST_OK)
                        {
                            DoScriptText(SAY_FIREBALL, m_creature);
                            m_uiMeteorTimer = 40000;
                        }
                    }
                }
                else
                    m_uiMeteorTimer -= uiDiff;

                // Switch to phase 2
                if (m_creature->GetHealthPercent() < 75.0f && m_uiPhase == PHASE_PHISYCAL_REALM)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_PHASING, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_PORTAL, CAST_TRIGGERED);
                        DoScriptText(SAY_PHASE_2, m_creature);

                        // inform controller about phase 2
                        if (m_pInstance)
                        {
                            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pController);

                            if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pHalion);
                        }
                        m_uiPhase = PHASE_TWILIGHT_REALM;
                    }
                }

                break;
            case PHASE_TWILIGHT_REALM:
                // this AI doesn't handle this phase
                break;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_halion_twilight
######*/

struct boss_halion_twilightAI : public ScriptedAI
{
    boss_halion_twilightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_ruby_sanctum*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiTailLashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiDarkBreathTimer;
    uint32 m_uiTwilightCutterTimer;;
    uint32 m_uiSoulConsumptionTimer;

    void Reset() override
    {
        m_uiPhase                = PHASE_PHISYCAL_REALM;
        m_uiTailLashTimer        = 10000;
        m_uiCleaveTimer          = urand(5000, 10000);
        m_uiDarkBreathTimer      = 15000;
        m_uiSoulConsumptionTimer = 20000;
        m_uiTwilightCutterTimer  = 30000;

    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, m_creature->GetObjectGuid(), 2);

        DoCastSpellIfCan(m_creature, SPELL_DUSK_SHROUD, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_PRECISION, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALION, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HALION, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            switch (urand(0, 1))
            {
                case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
                case 1: DoPlaySoundToSet(m_creature, SOUND_SLAY_2); break;
            }
        }
    }

    // share damage with the real version
    void DamageTaken(Unit* dealer, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (m_creature->GetHealth() <= damage)
            damage = m_creature->GetHealth() - 1;

        if (m_pInstance)
        {
            if (Creature* halion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_REAL))
                Unit::DealDamage(dealer, halion, damage, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        // phase 2 switch
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiPhase = PHASE_TWILIGHT_REALM;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_PHISYCAL_REALM:
                // nothing here - phase not handled by this npc
                break;
            case PHASE_BOTH_REALMS:
            // ToDo: handle corporeality
            // no break;
            case PHASE_TWILIGHT_REALM:

                if (m_uiTailLashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_LASH) == CAST_OK)
                        m_uiTailLashTimer = urand(15000, 25000);
                }
                else
                    m_uiTailLashTimer -= uiDiff;

                if (m_uiCleaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(10000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                if (m_uiDarkBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DARK_BREATH) == CAST_OK)
                        m_uiDarkBreathTimer = urand(15000, 20000);
                }
                else
                    m_uiDarkBreathTimer -= uiDiff;

                if (m_uiSoulConsumptionTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SOUL_CONSUMPTION, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SOUL_CONSUMPTION) == CAST_OK)
                            m_uiSoulConsumptionTimer = 25000;
                    }
                }
                else
                    m_uiSoulConsumptionTimer -= uiDiff;

                if (m_uiTwilightCutterTimer < uiDiff)
                {
                    if (m_pInstance)
                    {
                        DoScriptText(SAY_SPHERES, m_creature);

                        Creature* pOrb1 = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADOW_ORB_1);
                        Creature* pOrb2 = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADOW_ORB_2);

                        if (pOrb1 && pOrb2)
                        {
                            // self twilight pulse
                            pOrb1->CastSpell(pOrb1, SPELL_TWILIGHT_PULSE, TRIGGERED_OLD_TRIGGERED);
                            pOrb2->CastSpell(pOrb2, SPELL_TWILIGHT_PULSE, TRIGGERED_OLD_TRIGGERED);

                            // cutter
                            pOrb1->CastSpell(pOrb2, SPELL_TWILIGHT_CUTTER, TRIGGERED_NONE);
                        }

                        // heroic instance has 2 extra orbs
                        if (m_pInstance->IsHeroicDifficulty())
                        {
                            Creature* pOrb3 = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADOW_ORB_3);
                            Creature* pOrb4 = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADOW_ORB_4);

                            if (pOrb3 && pOrb4)
                            {
                                pOrb3->CastSpell(pOrb3, SPELL_TWILIGHT_PULSE, TRIGGERED_OLD_TRIGGERED);
                                pOrb4->CastSpell(pOrb4, SPELL_TWILIGHT_PULSE, TRIGGERED_OLD_TRIGGERED);

                                pOrb3->CastSpell(pOrb4, SPELL_TWILIGHT_CUTTER, TRIGGERED_NONE);
                            }
                        }
                    }

                    m_uiTwilightCutterTimer = 30000;
                }
                else
                    m_uiTwilightCutterTimer -= uiDiff;

                // Switch to phase 3
                if (m_creature->GetHealthPercent() < 50.0f && m_uiPhase == PHASE_TWILIGHT_REALM)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_DIVISION, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        // inform controller and halion about the phase change
                        if (m_pInstance)
                        {
                            if (Creature* pController = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pController);

                            if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_REAL))
                                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pHalion);
                        }

                        DoScriptText(SAY_PHASE_3, m_creature);
                        m_uiPhase = PHASE_BOTH_REALMS;
                    }
                }

                break;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_halion_controller
######*/

struct npc_halion_controllerAI : public ScriptedAI
{
    npc_halion_controllerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_ruby_sanctum*>(pCreature->GetInstanceData());
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint32 m_uiBerserkTimer;

    void Reset() override
    {
        m_uiBerserkTimer = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (!m_pInstance)
            return;

        // phase 2
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // Spawn the orbs and the carriers. Use the twilight Halion version to preserve the phase
            if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
            {
                pHalion->SummonCreature(NPC_ORB_ROTATION_FOCUS, aRotationFocusPosition[0], aRotationFocusPosition[1], aRotationFocusPosition[2], aRotationFocusPosition[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                pHalion->SummonCreature(NPC_ORB_CARRIER, aOrbCarrierPosition1[0], aOrbCarrierPosition1[1], aOrbCarrierPosition1[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0);
            }
        }
        // phase 3
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // update world states
            m_pInstance->DoUpdateWorldState(WORLD_STATE_CORPOREALITY, 1);
            m_pInstance->DoUpdateWorldState(WORLD_STATE_CORP_PHYSICAL, 50);
            m_pInstance->DoUpdateWorldState(WORLD_STATE_CORP_TWILIGHT, 50);

            // despawn phase 1 portal
            if (GameObject* pPortal = m_pInstance->GetSingleGameObjectFromStorage(GO_TWILIGHT_PORTAL_ENTER_1))
            {
                pPortal->SetForcedDespawn();
                pPortal->SetLootState(GO_JUST_DEACTIVATED);
            }

            // spawn phase 3 portals
            GuidList lPortalsGuids;
            m_pInstance->GetPortalsGuidList(lPortalsGuids);

            for (const auto& guid : lPortalsGuids)
                m_pInstance->DoRespawnGameObject(guid, 30 * MINUTE);
        }
        // set Berserk timer
        else if (eventType == AI_EVENT_CUSTOM_C)
            m_uiBerserkTimer = miscValue;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_REAL))
                    {
                        DoScriptText(SAY_BERSERK, pHalion);
                        pHalion->CastSpell(pHalion, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                    }

                    if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                    {
                        DoScriptText(SAY_BERSERK, pHalion);
                        pHalion->CastSpell(pHalion, SPELL_BERSERK, TRIGGERED_OLD_TRIGGERED);
                    }
                }

                m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }
    }
};

/*######
## npc_orb_carrier
######*/

struct npc_orb_carrierAI : public ScriptedAI
{
    npc_orb_carrierAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiTrackRotationTimer;

    void Reset() override
    {
        m_uiTrackRotationTimer = 1000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        // spell has attrribute: channel track target
        // this means that the caster will follow the target and automatically change the rotation
        // by changing the rotation the channel will get interrupted every 1 sec, so the caster has to be be forced to start the channel again
        if (m_uiTrackRotationTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TRACK_ROTATION, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                m_uiTrackRotationTimer = 1000;
        }
        else
            m_uiTrackRotationTimer -= uiDiff;
    }
};

/*######
## go_twilight_portal
######*/

bool GOUse_go_twilight_portal(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_HALION) != IN_PROGRESS)
        return true;

    // ToDo: check if this removes the real realm damage auras

    if (!pPlayer->HasAura(SPELL_TWILIGHT_REALM))
        pPlayer->CastSpell(pPlayer, SPELL_TWILIGHT_REALM, TRIGGERED_OLD_TRIGGERED);

    return false;
}

/*######
## spell_leave_twilight_realm_aura - 74812
######*/

struct spell_leave_twilight_realm_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            if (!target || !target->IsPlayer())
                return;

            if (target->HasAura(SPELL_TWILIGHT_REALM))
                target->RemoveAurasDueToSpell(SPELL_TWILIGHT_REALM);

            // ToDo: check if this removes the twilight realm damage auras
        }
    }
};

/*######
## spell_clear_debuffs - 75396
######*/

struct spell_clear_debuffs : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        uint32 uiSpell = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        if (target->HasAura(uiSpell))
            target->RemoveAurasDueToSpell(uiSpell);

        // remove other debuffs
        target->RemoveAurasDueToSpell(74562);
        target->RemoveAurasDueToSpell(74792);
    }
};

/*######
## spell_fiery_combustion_aura - 74562
######*/

struct spell_fiery_combustion_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (apply)
            target->CastSpell(target, 74567, TRIGGERED_OLD_TRIGGERED);
        else
        {
            target->RemoveAurasDueToSpell(74567);

            if (aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE || aura->GetRemoveMode() == AURA_REMOVE_BY_DISPEL)
            {
                target->CastSpell(target, 74607, TRIGGERED_OLD_TRIGGERED);
                target->CastSpell(target, 74610, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

/*######
## spell_soul_consumption_aura - 74792
######*/

struct spell_soul_consumption_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer())
            return;

        if (apply)
            target->CastSpell(target, 74795, TRIGGERED_OLD_TRIGGERED);
        else
        {
            target->RemoveAurasDueToSpell(74795);

            if (aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE || aura->GetRemoveMode() == AURA_REMOVE_BY_DISPEL)
            {
                target->CastSpell(target, 74799, TRIGGERED_OLD_TRIGGERED);
                target->CastSpell(target, 74800, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

void AddSC_boss_halion()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_halion_real";
    pNewScript->GetAI = &GetNewAIInstance<boss_halion_realAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_halion_twilight";
    pNewScript->GetAI = &GetNewAIInstance<boss_halion_twilightAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_halion_controller";
    pNewScript->GetAI = &GetNewAIInstance<npc_halion_controllerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orb_carrier";
    pNewScript->GetAI = &GetNewAIInstance<npc_orb_carrierAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_twilight_portal";
    pNewScript->pGOUse = &GOUse_go_twilight_portal;
    pNewScript->RegisterSelf();

    RegisterAuraScript<spell_leave_twilight_realm_aura>("spell_leave_twilight_realm_aura");
    RegisterSpellScript<spell_clear_debuffs>("spell_clear_debuffs");
    RegisterAuraScript<spell_fiery_combustion_aura>("spell_fiery_combustion_aura");
    RegisterAuraScript<spell_soul_consumption_aura>("spell_soul_consumption_aura");
}
