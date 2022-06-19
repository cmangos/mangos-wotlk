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
SDName: grand_champions
SD%Complete: 90
SDComment: Encounter might require additional improvements.
SDCategory: Crusader Coliseum, Trial of the Champion
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_champion.h"
#include "Maps/TransportSystem.h"

enum
{
    // common spells
    SPELL_DEFEND_DUMMY              = 64101,                        // triggers 62719, 64192

    SPELL_SHIELD_BREAKER            = 68504,
    SPELL_CHARGE                    = 68301,                        // triggers 68307
    SPELL_CHARGE_VEHICLE            = 68307,
    SPELL_FULL_HEAL                 = 43979,
    SPELL_RIDE_ARGENT_VEHICLE       = 69692,
};

/*######
## trial_companion_common
######*/

struct trial_companion_commonAI : public ScriptedAI
{
    trial_companion_commonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShieldBreakerTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiDefeatedTimer;
    uint32 m_uiResetThreatTimer;

    bool m_bDefeated;

    ObjectGuid m_newMountGuid;

    void Reset() override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            else
                DoCastSpellIfCan(m_creature, SPELL_DEFEND_DUMMY, CAST_TRIGGERED);
        }

        m_uiShieldBreakerTimer  = urand(3000, 5000);
        m_uiChargeTimer         = urand(1000, 3000);
        m_uiDefeatedTimer       = 0;
        m_uiResetThreatTimer    = urand(5000, 15000);

        m_bDefeated             = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
                m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);

            m_pInstance->DoSetChamptionsInCombat(pWho);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
                m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, FAIL);
        }
    }

    void AttackStart(Unit* pWho) override
    {
        ScriptedAI::AttackStart(pWho);

        // Set Mount control
        if (m_creature->GetTransportInfo() && m_creature->GetTransportInfo()->IsOnVehicle())
        {
            if (Creature* pMount = (Creature*)m_creature->GetTransportInfo()->GetTransport())
                pMount->AI()->AttackStart(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // no aggro during the intro
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_bDefeated)
                return;

            if (!m_pInstance)
                return;

            // second part of the champions challenge
            if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SetHealth(1);

                // no movement
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();

                // check if the other champions are wounded and set instance data
                if (m_pInstance->IsArenaChallengeComplete(TYPE_GRAND_CHAMPIONS))
                    m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
            }
            // first part of the champions challenge (arena encounter)
            else
            {
                // unmount
                if (Creature* pMount = (Creature*)m_creature->GetTransportInfo()->GetTransport())
                {
                    pMount->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                    pMount->ForcedDespawn();
                }

                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                m_creature->SetHealth(1);

                // no movement
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();

                m_uiDefeatedTimer = 15000;
            }

            m_bDefeated = true;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        switch (uiPointId)
        {
            case POINT_ID_MOUNT:
            {
                // mount the closest vehicle and start attacking
                uint32 uiMountEntry = m_pInstance->GetMountEntryForChampion();

                // search for the vehicle again, just in case the previous one was taken
                Creature* pMount = m_creature->GetMap()->GetCreature(m_newMountGuid);
                if (pMount->HasAura(SPELL_RIDE_ARGENT_VEHICLE))
                    pMount = GetClosestCreatureWithEntry(m_creature, uiMountEntry, 60.0f);

                // if we don't have any mount send an error
                if (!pMount)
                {
                    script_error_log("Instance Trial of the Champion: ERROR Failed to get a mount replacement for champion %u.", m_creature->GetEntry());
                    return;
                }

                DoCastSpellIfCan(pMount, SPELL_RIDE_ARGENT_VEHICLE, CAST_TRIGGERED);

                if (m_creature->GetVictim())
                    pMount->AI()->AttackStart(m_creature->GetVictim());

                m_bDefeated = false;
                break;
            }
            case POINT_ID_EXIT:
                // mark the first part as complete if required
                if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) != DONE)
                    m_pInstance->SetData(TYPE_ARENA_CHALLENGE, DONE);

                m_creature->ForcedDespawn();
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || uiPointId != POINT_ID_HOME || !m_pInstance)
            return;

        if (Creature* pCenterTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER))
        {
            pSummoned->SetRespawnCoord(pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), pSummoned->GetAngle(pCenterTrigger));
            pSummoned->SetFacingToObject(pCenterTrigger);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_uiShieldBreakerTimer = urand(1000, 2000);
            m_uiChargeTimer = urand(2000, 4000);
        }
    }

    // function that will make the champion to use the nearby available mount
    void DoUseNearbyMountIfCan()
    {
        if (!m_pInstance)
            return;

        // set instance data as special if first part is completed
        if (m_pInstance->IsArenaChallengeComplete(TYPE_ARENA_CHALLENGE))
            m_pInstance->SetData(TYPE_ARENA_CHALLENGE, SPECIAL);
        else
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            float fX, fY, fZ;
            uint32 uiMountEntry = m_pInstance->GetMountEntryForChampion();

            if (Creature* pMount = GetClosestCreatureWithEntry(m_creature, uiMountEntry, 60.0f))
            {
                pMount->GetContactPoint(m_creature, fX, fY, fZ);
                m_creature->SetWalk(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_MOUNT, fX, fY, fZ);

                m_newMountGuid = pMount->GetObjectGuid();
            }
        }
    }

    // Return true to handle shared timers and MeleeAttack
    virtual bool UpdateChampionAI(const uint32 /*uiDiff*/) { return true; }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // timer for other champions check
        if (m_uiDefeatedTimer)
        {
            if (m_uiDefeatedTimer <= uiDiff)
            {
                DoUseNearbyMountIfCan();
                m_uiDefeatedTimer = 0;
            }
            else
                m_uiDefeatedTimer -= uiDiff;
        }

        // no combat after defeated
        if (m_bDefeated)
            return;

        if (!m_pInstance)
            return;

        // arena battle - on vehicles
        if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == IN_PROGRESS)
        {
            if (m_uiShieldBreakerTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHIELD_BREAKER) == CAST_OK)
                    m_uiShieldBreakerTimer = urand(2000, 4000);
            }
            else
                m_uiShieldBreakerTimer -= uiDiff;

            if (m_uiChargeTimer)
            {
                if (m_uiChargeTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHARGE) == CAST_OK)
                    {
                        if (m_creature->GetTransportInfo() && m_creature->GetTransportInfo()->IsOnVehicle())
                        {
                            if (Creature* pMount = (Creature*)m_creature->GetTransportInfo()->GetTransport())
                                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature->GetVictim(), pMount);
                        }
                        m_uiChargeTimer = 0;
                    }
                }
                else
                    m_uiChargeTimer -= uiDiff;
            }
        }
        // arena challenge complete - start normal battle
        else if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
        {
            // Call specific virtual function
            if (!UpdateChampionAI(uiDiff))
                return;

            // Change target
            if (m_uiResetThreatTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
                    DoResetThreat();
                    AttackStart(pTarget);
                    m_uiResetThreatTimer = urand(5000, 15000);
                }
            }
            else
                m_uiResetThreatTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

enum
{
    // warrior spells
    SPELL_INTERCEPT             = 67540,
    SPELL_BLADESTORM            = 67541,
    SPELL_MORTAL_STRIKE         = 67542,
    SPELL_ROLLING_THROW         = 67546,
};

/*######
## boss_champion_warrior
######*/

struct boss_champion_warriorAI : public trial_companion_commonAI
{
    boss_champion_warriorAI(Creature* pCreature) : trial_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiStrikeTimer;
    uint32 m_uiBladeStormTimer;
    uint32 m_uiInterceptTimer;
    uint32 m_uiThrowTimer;

    void Reset() override
    {
        m_uiInterceptTimer      = 0;
        m_uiStrikeTimer         = urand(5000, 8000);
        m_uiBladeStormTimer     = urand(10000, 20000);
        m_uiThrowTimer          = 30000;

        trial_companion_commonAI::Reset();
    }

    bool UpdateChampionAI(const uint32 uiDiff)
    {
        if (m_uiInterceptTimer < uiDiff)
        {
            if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_INTERCEPT, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_INTERCEPT) == CAST_OK)
                        m_uiInterceptTimer = 10000;
                }
            }
            else
                m_uiInterceptTimer = 2000;
        }
        else
            m_uiInterceptTimer -= uiDiff;

        if (m_uiBladeStormTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLADESTORM) == CAST_OK)
                m_uiBladeStormTimer = urand(15000, 20000);
        }
        else
            m_uiBladeStormTimer -= uiDiff;

        if (m_uiThrowTimer < uiDiff)
        {
            m_creature->GetVictim()->CastSpell(m_creature->GetVictim(), SPELL_ROLLING_THROW, TRIGGERED_OLD_TRIGGERED);
            m_uiThrowTimer = urand(20000, 30000);
        }
        else
            m_uiThrowTimer -= uiDiff;

        if (m_uiStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MORTAL_STRIKE) == CAST_OK)
                m_uiStrikeTimer = urand(8000, 12000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        return true;
    }
};

UnitAI* GetAI_boss_champion_warrior(Creature* pCreature)
{
    return new boss_champion_warriorAI(pCreature);
}

enum
{
    // mage spells
    SPELL_FIREBALL              = 66042,
    SPELL_POLYMORPH             = 66043,
    SPELL_BLAST_WAVE            = 66044,
    SPELL_HASTE                 = 66045,
};

/*######
## boss_champion_mage
######*/

struct boss_champion_mageAI : public trial_companion_commonAI
{
    boss_champion_mageAI(Creature* pCreature) : trial_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiFireballTimer;
    uint32 m_uiBlastWaveTimer;
    uint32 m_uiHasteTimer;
    uint32 m_uiPolymorphTimer;

    void Reset() override
    {
        m_uiFireballTimer       = 0;
        m_uiBlastWaveTimer      = urand(10000, 20000);
        m_uiHasteTimer          = 10000;
        m_uiPolymorphTimer      = urand(5000, 10000);

        trial_companion_commonAI::Reset();
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
            m_attackDistance = 20.0f;
        
        trial_companion_commonAI::AttackStart(pWho);
    }

    bool UpdateChampionAI(const uint32 uiDiff)
    {
        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireballTimer = urand(2000, 4000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiBlastWaveTimer < uiDiff)
        {
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BLAST_WAVE, SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE) == CAST_OK)
                    m_uiBlastWaveTimer = urand(10000, 20000);
            }
            else
                m_uiBlastWaveTimer = 5000;
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        if (m_uiHasteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HASTE) == CAST_OK)
                m_uiHasteTimer = 20000;
        }
        else
            m_uiHasteTimer -= uiDiff;

        if (m_uiPolymorphTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_POLYMORPH) == CAST_OK)
                    m_uiPolymorphTimer = urand(5000, 10000);
            }
        }
        else
            m_uiPolymorphTimer -= uiDiff;

        return true;
    }
};

UnitAI* GetAI_boss_champion_mage(Creature* pCreature)
{
    return new boss_champion_mageAI(pCreature);
}

enum
{
    // shaman spells
    SPELL_HEALING_WAVE          = 67528,
    SPELL_CHAIN_LIGHTNING       = 67529,
    SPELL_EARTH_SHIELD          = 67530,
    SPELL_HEX_OF_MENDING        = 67534,
};

/*######
## boss_champion_shaman
######*/

struct boss_champion_shamanAI : public trial_companion_commonAI
{
    boss_champion_shamanAI(Creature* pCreature) : trial_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiHealingWaveTimer;
    uint32 m_uiLightningTimer;
    uint32 m_uiEarthShieldTimer;
    uint32 m_uiHexTimer;

    void Reset() override
    {
        m_uiLightningTimer          = 1000;
        m_uiHealingWaveTimer        = 13000;
        m_uiHexTimer                = 10000;
        m_uiEarthShieldTimer        = 0;

        trial_companion_commonAI::Reset();
    }

    bool UpdateChampionAI(const uint32 uiDiff)
    {
        if (m_uiLightningTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                m_uiLightningTimer = urand(1000, 3000);
        }
        else
            m_uiLightningTimer -= uiDiff;

        if (m_uiHealingWaveTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HEALING_WAVE) == CAST_OK)
                    m_uiHealingWaveTimer = urand(8000, 13000);
            }
        }
        else
            m_uiHealingWaveTimer -= uiDiff;

        if (m_uiEarthShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EARTH_SHIELD, CAST_AURA_NOT_PRESENT) == CAST_OK)
                m_uiEarthShieldTimer = 30000;
            else
                m_uiEarthShieldTimer = 5000;
        }
        else
            m_uiEarthShieldTimer -= uiDiff;

        if (m_uiHexTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEX_OF_MENDING) == CAST_OK)
                m_uiHexTimer = urand(17000, 25000);
        }
        else
            m_uiHexTimer -= uiDiff;

        return true;
    }
};

UnitAI* GetAI_boss_champion_shaman(Creature* pCreature)
{
    return new boss_champion_shamanAI(pCreature);
}

enum
{
    // hunter spells
    SPELL_DISENGAGE             = 68340,                // trigger 68340
    SPELL_LIGHTNING_ARROWS      = 66083,
    SPELL_LIGHTNING_ARROWS_PROC = 66085,
    SPELL_MULTI_SHOT            = 66081,
    SPELL_SHOOT                 = 65868,
};

/*######
## boss_champion_hunter
######*/

struct boss_champion_hunterAI : public trial_companion_commonAI
{
    boss_champion_hunterAI(Creature* pCreature) : trial_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiDisengageTimer;
    uint32 m_uiArrowsTimer;
    uint32 m_uiArrowsProcTimer;
    uint32 m_uiMultiShotTimer;
    uint32 m_uiShootTimer;

    void Reset() override
    {
        m_uiShootTimer              = 1000;
        m_uiArrowsTimer             = urand(10000, 15000);
        m_uiArrowsProcTimer         = 0;
        m_uiMultiShotTimer          = urand(6000, 12000);
        m_uiDisengageTimer          = 5000;

        trial_companion_commonAI::Reset();
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ARENA_CHALLENGE) == DONE)
            m_attackDistance = 20.0f;

        trial_companion_commonAI::AttackStart(pWho);
    }

    bool UpdateChampionAI(const uint32 uiDiff)
    {
        if (m_uiShootTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOOT) == CAST_OK)
                m_uiShootTimer = urand(1000, 3000);
        }
        else
            m_uiShootTimer -= uiDiff;

        if (m_uiMultiShotTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MULTI_SHOT) == CAST_OK)
                m_uiMultiShotTimer = urand(5000, 10000);
        }
        else
            m_uiMultiShotTimer -= uiDiff;

        if (m_uiDisengageTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DISENGAGE, SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DISENGAGE) == CAST_OK)
                    m_uiDisengageTimer = urand(13000, 18000);
            }
            else
                m_uiDisengageTimer = 5000;
        }
        else
            m_uiDisengageTimer -= uiDiff;

        if (m_uiArrowsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_ARROWS) == CAST_OK)
            {
                m_uiArrowsTimer = urand(23000, 27000);
                m_uiArrowsProcTimer = 3000;
            }
        }
        else
            m_uiArrowsTimer -= uiDiff;

        if (m_uiArrowsProcTimer)
        {
            if (m_uiArrowsProcTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_ARROWS_PROC) == CAST_OK)
                    m_uiArrowsProcTimer = 0;
            }
            else
                m_uiArrowsProcTimer -= uiDiff;
        }

        return true;
    }
};

UnitAI* GetAI_boss_champion_hunter(Creature* pCreature)
{
    return new boss_champion_hunterAI(pCreature);
}

enum
{
    // rogue spells
    SPELL_POISON_BOTTLE         = 67701,
    SPELL_FAN_OF_KNIVES         = 67706,
    SPELL_EVISCERATE            = 67709,
    SPELL_DEADLY_POISON         = 67710,
};

/*######
## boss_champion_rogue
######*/

struct boss_champion_rogueAI : public trial_companion_commonAI
{
    boss_champion_rogueAI(Creature* pCreature) : trial_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiPoisonBottleTimer;
    uint32 m_uiFanKnivesTimer;
    uint32 m_uiEviscerateTimer;
    uint32 m_uiDeadlyPoisonTimer;

    void Reset() override
    {
        m_uiDeadlyPoisonTimer       = 12000;
        m_uiEviscerateTimer         = 7000;
        m_uiFanKnivesTimer          = 10000;
        m_uiPoisonBottleTimer       = 5000;

        trial_companion_commonAI::Reset();
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiPoisonBottleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_POISON_BOTTLE) == CAST_OK)
                m_uiPoisonBottleTimer = urand(15000, 20000);
        }
        else
            m_uiPoisonBottleTimer -= uiDiff;

        if (m_uiDeadlyPoisonTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEADLY_POISON) == CAST_OK)
                m_uiDeadlyPoisonTimer = urand(9000, 15000);
        }
        else
            m_uiDeadlyPoisonTimer -= uiDiff;

        if (m_uiEviscerateTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_EVISCERATE) == CAST_OK)
                m_uiEviscerateTimer = 8000;
        }
        else
            m_uiEviscerateTimer -= uiDiff;

        if (m_uiFanKnivesTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FAN_OF_KNIVES) == CAST_OK)
                m_uiFanKnivesTimer = urand(10000, 15000);
        }
        else
            m_uiFanKnivesTimer -= uiDiff;

        return true;
    }
};

UnitAI* GetAI_boss_champion_rogue(Creature* pCreature)
{
    return new boss_champion_rogueAI(pCreature);
}

/*######
## npc_trial_grand_champion
######*/

enum
{
    SPELL_CHAMPION_CHARGE           = 63010,
    SPELL_CHAMPION_DEFEND           = 64100,
};

struct npc_trial_grand_championAI : public ScriptedAI
{
    npc_trial_grand_championAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;

    uint32 m_uiChargeTimer;
    uint32 m_uiBlockTimer;
    uint32 m_uiChargeResetTimer;

    void Reset() override
    {
        m_uiChargeTimer         = 1000;
        m_uiBlockTimer          = 0;
        m_uiChargeResetTimer    = 0;
        m_attackDistance = frand(10.0f, 20.0f);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBlockTimer < uiDiff)
        {
            if (!m_creature->HasAura(SPELL_CHAMPION_DEFEND))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CHAMPION_DEFEND) == CAST_OK)
                    m_uiBlockTimer = 7000;
            }
            else
                m_uiBlockTimer = 2000;
        }
        else
            m_uiBlockTimer -= uiDiff;

        if (m_uiChargeTimer)
        {
            if (m_uiChargeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAMPION_CHARGE) == CAST_OK)
                {
                    DoStartMovement(m_creature->GetVictim());
                    m_uiChargeResetTimer = urand(5000, 10000);
                    m_uiChargeTimer = 0;
                }
            }
            else
                m_uiChargeTimer -= uiDiff;
        }

        if (m_uiChargeResetTimer)
        {
            if (m_uiChargeResetTimer <= uiDiff)
            {
                m_attackDistance = frand(10.0f, 20.0f);
                DoStartMovement(m_creature->GetVictim());
                m_uiChargeResetTimer = 0;
                m_uiChargeTimer = urand(2000, 4000);
            }
            else
                m_uiChargeResetTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_trial_grand_champion(Creature* pCreature)
{
    return new npc_trial_grand_championAI(pCreature);
}

/*######
## npc_champion_mount
######*/

struct npc_champion_mountAI : public ScriptedAI
{
    npc_champion_mountAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;

    uint32 m_uiChargeResetTimer;

    ObjectGuid m_ownerGuid;

    void Reset() override
    {
        m_uiChargeResetTimer = 0;
        m_attackDistance = frand(10.0f, 20.0f);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        switch (uiPointId)
        {
            case POINT_ID_CENTER:
                m_pInstance->MoveChampionToHome(m_creature);
                break;
            case POINT_ID_HOME:
                if (Creature* pCenterTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER))
                {
                    m_creature->SetRespawnCoord(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetAngle(pCenterTrigger));
                    m_creature->SetFacingToObject(pCenterTrigger);
                }
                m_pInstance->InformChampionReachHome();
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoCastSpellIfCan(pInvoker, SPELL_CHARGE_VEHICLE, CAST_TRIGGERED);
            DoStartMovement(pInvoker);
            m_ownerGuid = pSender->GetObjectGuid();
            m_uiChargeResetTimer = urand(5000, 10000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiChargeResetTimer)
        {
            if (m_uiChargeResetTimer <= uiDiff)
            {
                if (Creature* pOwner = m_creature->GetMap()->GetCreature(m_ownerGuid))
                    SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pOwner);

                m_attackDistance = frand(10.0f, 20.0f);
                DoStartMovement(m_creature->GetVictim());
                m_uiChargeResetTimer = 0;
            }
            else
                m_uiChargeResetTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_champion_mount(Creature* pCreature)
{
    return new npc_champion_mountAI(pCreature);
}

void AddSC_boss_grand_champions()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_champion_warrior";
    pNewScript->GetAI = &GetAI_boss_champion_warrior;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_champion_mage";
    pNewScript->GetAI = &GetAI_boss_champion_mage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_champion_shaman";
    pNewScript->GetAI = &GetAI_boss_champion_shaman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_champion_hunter";
    pNewScript->GetAI = &GetAI_boss_champion_hunter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_champion_rogue";
    pNewScript->GetAI = &GetAI_boss_champion_rogue;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_trial_grand_champion";
    pNewScript->GetAI = &GetAI_npc_trial_grand_champion;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_champion_mount";
    pNewScript->GetAI = &GetAI_npc_champion_mount;
    pNewScript->RegisterSelf();
}
