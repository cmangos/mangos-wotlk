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
SDComment: Phase 3 and related transition NYI; Twilight portals NYI; Shadow Orbs NYI; Meteor Strikes NYI; Heroic abilities NYI.
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

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

    // Transitions
    SPELL_TWILIGHT_PHASING      = 74808,                    // Start phase 2
    SPELL_SUMMON_PORTAL         = 74809,
    SPELL_TWILIGHT_DIVISION     = 75063,
    SPELL_TWILIGHT_REALM        = 74807,
    SPELL_TWILIGHT_MENDING      = 75509,
    SPELL_LEAVE_TWILIGHT_REALM  = 74812,                    // handled by GO 202796
    //share damage spell: 74810 - serverside spell

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
    //Combustion
    SPELL_COMBUSTION_PERIODIC   = 74629,                    // cast by npc 40001

    // Consumption
    SPELL_CONSUMPTION_PERIODIC  = 74803,                    // cast by npc 40135

    // Meteor
    SPELL_METEOR_VISUAL         = 74641,                    // cast by npc 40029 (all meteor spells)
    SPELL_METEOR_IMPACT         = 74648,                    // cast on visual aura expire
    SPELL_METEOR_FLAME          = 74713,
    SPELL_METEOR_FLAME2         = 74718,                    // cast by the secondary strike npcs
    SPELL_BIRTH                 = 40031,                    // cast by the meteor strike npcs

    // Cutter
    SPELL_TWILIGHT_CUTTER       = 74768,
    SEPLL_TWILIGHT_PULSE        = 78861,
    SPELL_TRACK_ROTATION        = 74758,                    // cast by 40081 on 40091

    // Living Inferno
    SPELL_BLAZING_AURA          = 75885,                    // cast by 40681

    // Living Ember
    SPELL_AWAKEN_FLAMES         = 75889,                    // cast by 40683

    // Npcs
    NPC_COMBUSTION              = 40001,
    NPC_METEOR_STRIKE_MAIN      = 40029,                    // summons the other meteor strikes using serverside spells like 74680, 74681, 74682, 74683
    NPC_CONSUMPTION             = 40135,
    NPC_ORB_CARRIER             = 40081,                    // vehicle for shadow orbs
    NPC_ORB_ROTATION_FOCUS      = 40091,

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

static const float aRotationFocusPosition[4] = {3113.711f, 533.5382f, 72.96f, 1.93f};
static const float aOrbCarrierPosition1[3] = {3153.75f, 579.1875f, 70.47f};
static const float aOrbCarrierPosition2[3] = {3153.75f, 487.1875f, 70.47f};

/*######
## boss_halion_real
######*/

struct boss_halion_realAI : public ScriptedAI
{
    boss_halion_realAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruby_sanctum*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint8 m_uiPhase;

    uint32 m_uiTailLashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiFieryCombustionTimer;
    uint32 m_uiMeteorTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiBerserkTimer;

    void Reset() override
    {
        m_uiPhase                   = PHASE_PHISYCAL_REALM;

        m_uiTailLashTimer           = 10000;
        m_uiCleaveTimer             = urand(5000, 10000);
        m_uiFieryCombustionTimer    = 15000;
        m_uiMeteorTimer             = 20000;
        m_uiBerserkTimer            = 8 * MINUTE * IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_HALION, IN_PROGRESS);
            m_pInstance->SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, m_creature->GetObjectGuid(), 1);
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
            case NPC_COMBUSTION:
                pSummoned->CastSpell(pSummoned, SPELL_COMBUSTION_PERIODIC, true);
                break;
            case NPC_METEOR_STRIKE_MAIN:
                // ToDo: summon the other meteor strikes around this one
                pSummoned->CastSpell(pSummoned, SPELL_BIRTH, true);
                pSummoned->CastSpell(pSummoned, SPELL_METEOR_VISUAL, true);
                break;
        }
    }

    void DoPrepareTwilightPhase()
    {
        if (!m_pInstance)
            return;

        // Spawn the orbs and the carriers. Use the twilight Halion version to preserve the phase
        if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
        {
            // Set current Halion hp
            pHalion->SetHealth(m_creature->GetHealth());

            // NOTE: the spawn coords seem to be totally off, compared to the actual map layout - requires additional research!!!

            // Spawn the rotation focus first
            // pHalion->SummonCreature(NPC_ORB_ROTATION_FOCUS, aRotationFocusPosition[0], aRotationFocusPosition[1], aRotationFocusPosition[2], aRotationFocusPosition[3], TEMPSUMMON_DEAD_DESPAWN, 0);

            // Then spawn the orb carriers and the shadow orbs. ToDo: research if it's possible to make this dynamic
            // pHalion->SummonCreature(NPC_ORB_CARRIER, aOrbCarrierPosition1[0], aOrbCarrierPosition1[1], aOrbCarrierPosition1[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            // pHalion->SummonCreature(NPC_ORB_CARRIER, aOrbCarrierPosition2[0], aOrbCarrierPosition2[1], aOrbCarrierPosition2[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            // pHalion->SummonCreature(NPC_SHADOW_ORB_1, aOrbCarrierPosition1[0], aOrbCarrierPosition1[1], aOrbCarrierPosition1[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            // pHalion->SummonCreature(NPC_SHADOW_ORB_2, aOrbCarrierPosition2[0], aOrbCarrierPosition2[1], aOrbCarrierPosition2[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    // Do the same for the Twilight halion
                    if (m_pInstance)
                    {
                        if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT, true))
                            pHalion->CastSpell(pHalion, SPELL_BERSERK, true);
                    }

                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

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
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
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
                        DoPrepareTwilightPhase();
                        m_uiPhase = PHASE_TWILIGHT_REALM;
                    }
                }

                break;
            case PHASE_TWILIGHT_REALM:

                // Switch to phase 3
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_PHASING);
                    DoScriptText(SAY_PHASE_3, m_creature);
                    m_uiPhase = PHASE_BOTH_REALMS;
                }

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
        m_pInstance = (instance_ruby_sanctum*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruby_sanctum* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiTailLashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiDarkBreathTimer;
    uint32 m_uiSoulConsumptionTimer;

    void Reset() override
    {
        m_uiPhase                = PHASE_TWILIGHT_REALM;
        m_uiTailLashTimer        = 10000;
        m_uiCleaveTimer          = urand(5000, 10000);
        m_uiDarkBreathTimer      = 15000;
        m_uiSoulConsumptionTimer = 20000;
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
        // Allow real Halion to evade
        if (m_pInstance)
        {
            if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_REAL))
                pHalion->AI()->EnterEvadeMode();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // ToDo: handle the damage sharing!

        DoScriptText(SAY_DEATH, m_creature);
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

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CONSUMPTION:
                pSummoned->CastSpell(pSummoned, SPELL_CONSUMPTION_PERIODIC, true);
                break;
            case NPC_SHADOW_ORB_1:
            case NPC_SHADOW_ORB_2:
            case NPC_SHADOW_ORB_3:
            case NPC_SHADOW_ORB_4:
                if (Creature* pCarrier = GetClosestCreatureWithEntry(pSummoned, NPC_ORB_CARRIER, 5.0f))
                    pSummoned->CastSpell(pCarrier, SPELL_RIDE_VEHICLE_HARDCODED, true);
                break;
            case NPC_ORB_CARRIER:
                pSummoned->CastSpell(pSummoned, SPELL_TRACK_ROTATION, true);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
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
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(10000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                if (m_uiDarkBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_BREATH) == CAST_OK)
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

                // Switch to phase 3
                if (m_creature->GetHealthPercent() < 50.0f && m_uiPhase == PHASE_TWILIGHT_REALM)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TWILIGHT_DIVISION, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        if (m_pInstance)
                        {
                            // ToDo: Update world states and spawn the exit portals

                            // Set the real Halion health, so it can also begin phase 3
                            if (Creature* pHalion = m_pInstance->GetSingleCreatureFromStorage(NPC_HALION_REAL))
                                pHalion->SetHealth(m_creature->GetHealth());
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

CreatureAI* GetAI_boss_halion_real(Creature* pCreature)
{
    return new boss_halion_realAI(pCreature);
};

CreatureAI* GetAI_boss_halion_twilight(Creature* pCreature)
{
    return new boss_halion_twilightAI(pCreature);
};

void AddSC_boss_halion()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_halion_real";
    pNewScript->GetAI = &GetAI_boss_halion_real;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_halion_twilight";
    pNewScript->GetAI = &GetAI_boss_halion_twilight;
    pNewScript->RegisterSelf();
}
