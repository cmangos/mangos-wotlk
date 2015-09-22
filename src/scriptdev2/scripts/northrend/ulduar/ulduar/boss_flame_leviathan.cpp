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
SDName: boss_flame_leviathan
SD%Complete: 75%
SDComment: Defense turret AI and related event NYI.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                               = -1603159,
    SAY_SLAY                                = -1603160,
    SAY_DEATH                               = -1603161,
    SAY_CHANGE_1                            = -1603162,
    SAY_CHANGE_2                            = -1603163,
    SAY_CHANGE_3                            = -1603164,

    SAY_PLAYER_RIDE                         = -1603165,
    SAY_OVERLOAD_1                          = -1603166,
    SAY_OVERLOAD_2                          = -1603167,
    SAY_OVERLOAD_3                          = -1603168,

    SAY_HARD_MODE                           = -1603169,
    SAY_TOWER_FROST                         = -1603170,
    SAY_TOWER_FIRE                          = -1603171,
    SAY_TOWER_ENERGY                        = -1603172,
    SAY_TOWER_NATURE                        = -1603173,
    SAY_TOWER_DOWN                          = -1603174,

    EMOTE_PURSUE                            = -1603175,
    EMOTE_HODIR_FURY                        = -1603242,
    EMOTE_FREYA_WARD                        = -1603243,
    EMOTE_MIMIRON_INFERNO                   = -1603244,
    EMOTE_THORIM_HAMMER                     = -1603245,

    // Leviathan spells
    SPELL_INVISIBILITY_DETECTION            = 18950,
    SPELL_PURSUED                           = 62374,
    SPELL_MISSILE_BARRAGE_TARGET            = 62401,                    // triggers 62400 on target
    SPELL_MISSILE_BARRAGE                   = 62400,                    // has radius of 5k; requires core update
    SPELL_FLAME_VENTS                       = 62396,
    SPELL_BATTERING_RAM                     = 62376,
    SPELL_GATHERING_SPEED                   = 62375,

    // leviathan turret spells - handled in eventAI
    // SPELL_FLAME_CANNON                   = 62395,

    // defense turret spells
    SPELL_OVERLOAD_CIRCUIT                  = 62399,
    SPELL_SEARING_FLAME                     = 62402,
    SPELL_SYSTEMS_SHUTDOWN                  = 62475,                    // sends event 21605 for achiev check
    // Leviathan seat has missing aura 62421

    // leviathan other spells - for the moment these are not used
    // SPELL_SMOKE_TRAIL                    = 63575,
    // SPELL_EJECT_ALL_PASSENGERS           = 50630,                    // used by vehicles on death; currently handled by DB linking
    // SPELL_EJECT_PASSENGER_4              = 64614,
    // SPELL_EJECT_PASSENGER_1              = 60603,

    // tower buffs to Leviathan (applied on combat start if the towers are alive)
    SPELL_TOWER_OF_FROST                    = 65077,
    // SPELL_TOWER_OF_FROST_DEBUFF          = 65079,                    // removed by hotfix
    SPELL_TOWER_OF_LIFE                     = 64482,
    SPELL_TOWER_OF_STORMS                   = 65076,
    SPELL_TOWER_OF_FLAMES                   = 65075,

    // tower beacon abilities
    SPELL_HODIR_FURY                        = 62533,                    // tower of frost
    SPELL_FREYA_WARD                        = 62906,                    // tower of life
    SPELL_THORIM_HAMMER                     = 62911,                    // tower of storms
    SPELL_MIMIRON_INFERNO                   = 62909,                    // tower of flames

    // beacon vehicles visuals
    SPELL_LIGHTNING_SKYBEAM                 = 62897,                    // storm beacon
    SPELL_RED_SKYBEAM                       = 63772,                    // flames beacon
    SPELL_BLUE_SKYBEAM                      = 63769,                    // frost beacon
    SPELL_GREEN_SKYBEAM                     = 62895,                    // life beacon

    // other beacon vehicles spells
    // SPELL_TARGET_SEARCH_A                = 63761,                    // cast by npc 33369 and targets missing npc 33835
    // SPELL_TARGET_SEARCH_B                = 63762,                    // moves the caster to the target location
    // SPELL_TARGET_SEARCH_C                = 63763,                    // these are not used since we are doing this by waypoint movement
    // SPELL_TARGET_SEARCH_D                = 63764,
    // SPELL_BEAM_TARGET_STATE              = 62898,                    // cast by all tower reticles; purpose unk
    // SPELL_BIRTH                          = 40031,                    // not used; purpose unk

    // vehicle accessories
    // NPC_LEVIATHAN_SEAT                   = 33114,
    // NPC_LEVIATHAN_TURRET                 = 33139,
    // NPC_DEFENSE_TURRET                   = 33142,
    // NPC_OVERLOAD_DEVICE                  = 33143,

    // hard mode beacons - they cast the actual damage spells
    NPC_HODIR_FURY                          = 33212,
    // NPC_FREYA_WARD                       = 33367,
    // NPC_THORIM_HAMMER                    = 33365,                    // handled in eventAI
    // NPC_MIMIRON_INFERNO                  = 33370,

    // beacon vehicles
    NPC_THORIM_HAMMER_VEHICLE               = 33364,                    // has accessory 33365
    NPC_MIMIRON_INFERNO_VEHICLE             = 33369,                    // has accessory 33370
    NPC_HODIR_FURY_VEHICLE                  = 33108,                    // has accessory 33212
    NPC_FREYA_WARD_VEHICLE                  = 33366,                    // has accessory 33367

    // freya's ward summons - handled by eventAI
    NPC_WRITHING_LASHER                     = 33387,                    // both spam spell 65062 on target
    NPC_WARD_OF_LIFE                        = 34275,

    // other npcs (spawned at epilogue)
    SPELL_RIDE_VEHICLE                      = 43671,
    NPC_BRANN_FLYING_MACHINE                = 34120,
    NPC_BRANN_BRONZEBEARD_LEVIATHAN         = 34119,
    NPC_ARCHMANGE_RHYDIAN                   = 33696,

    MAX_FREYA_WARD                          = 4,
    MAX_HODIR_FURY                          = 2,
    // Mimiron inferno is only one
    MAX_THORIM_HAMMER                       = 22,

    TOWER_ID_HODIR                          = 0,
    TOWER_ID_FREYA                          = 1,
    TOWER_ID_MIMIRON                        = 2,
    TOWER_ID_THORIM                         = 3,
};

static const int32 aLeviathanTowerYell[KEEPER_ENCOUNTER] = { SAY_TOWER_FROST, SAY_TOWER_NATURE, SAY_TOWER_FIRE, SAY_TOWER_ENERGY };
static const int32 aLeviathanTowerEmote[KEEPER_ENCOUNTER] = { EMOTE_HODIR_FURY, EMOTE_FREYA_WARD, EMOTE_MIMIRON_INFERNO, EMOTE_THORIM_HAMMER };

static const float afFreyaWard[MAX_FREYA_WARD][4] =
{
    {156.9291f, 61.52306f, 409.887f, 5.68f},
    {376.641f, 68.61361f, 411.2287f, 3.85f},
    {383.6206f, -130.8576f, 410.7088f, 2.26f},
    {154.9095f, -137.4339f, 409.887f, 0.79f},
};

static const float afHodirFury[MAX_HODIR_FURY][3] =
{
    {219.9013f, 7.913357f, 409.7861f},
    {326.0777f, -74.99034f, 409.887f},
};

static const float afMimironInferno[3] = {329.1809f, 8.02577f, 409.887f};

/*######
## boss_flame_leviathan
######*/

struct boss_flame_leviathanAI : public ScriptedAI
{
    boss_flame_leviathanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bInitTowers = false;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bInitTowers;
    bool m_bUlduarTower[KEEPER_ENCOUNTER];

    uint32 m_uiBatteringRamTimer;
    uint32 m_uiFlameVentsTimer;
    uint32 m_uiMissileBarrageTimer;
    uint32 m_uiPursueTimer;
    uint32 m_uiGatheringSpeedTimer;

    uint32 m_uiHardModeTimer;
    uint8 m_uiHardModeStep;

    uint8 m_uiThorimHammerCount;
    uint32 m_uiThorimHammerTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_INVISIBILITY_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        for (uint8 i = 0; i < KEEPER_ENCOUNTER; ++i)
            m_bUlduarTower[i] = false;

        m_uiBatteringRamTimer   = 10000;
        m_uiFlameVentsTimer     = 30000;
        m_uiMissileBarrageTimer = 1000;
        m_uiPursueTimer         = 1000;
        m_uiGatheringSpeedTimer = 10000;

        m_uiHardModeTimer       = 10000;
        m_uiHardModeStep        = 0;
        m_uiThorimHammerCount   = 0;
        m_uiThorimHammerTimer   = 0;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_LEVIATHAN, DONE);

            // clear hard mode auras
            if (Creature* pOrbital = m_pInstance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT))
                pOrbital->RemoveAllAuras();
        }

        DoScriptText(SAY_DEATH, m_creature);

        // start epilogue event
        if (Creature* pFlyMachine = m_creature->SummonCreature(NPC_BRANN_FLYING_MACHINE, 175.2838f, -210.4325f, 501.2375f, 1.42f, TEMPSUMMON_CORPSE_DESPAWN, 0))
        {
            if (Creature* pBrann = m_creature->SummonCreature(NPC_BRANN_BRONZEBEARD_LEVIATHAN, 175.2554f, -210.6305f, 500.7375f, 1.42f, TEMPSUMMON_CORPSE_DESPAWN, 0))
                pBrann->CastSpell(pFlyMachine, SPELL_RIDE_VEHICLE, true);

            pFlyMachine->SetWalk(false);
            pFlyMachine->GetMotionMaster()->MovePoint(1, 229.9419f, -130.3764f, 409.5681f);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // check the towers again to make sure that some of them were not destroyed in the meanwhile
        FetchTowers();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEVIATHAN, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_SLAY, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEVIATHAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_THORIM_HAMMER_VEHICLE:
                pSummoned->CastSpell(pSummoned, SPELL_LIGHTNING_SKYBEAM, true);
                break;
            case NPC_MIMIRON_INFERNO_VEHICLE:
                pSummoned->CastSpell(pSummoned, SPELL_RED_SKYBEAM, true);
                break;
            case NPC_HODIR_FURY_VEHICLE:
                pSummoned->CastSpell(pSummoned, SPELL_BLUE_SKYBEAM, true);
                break;
            case NPC_FREYA_WARD_VEHICLE:
                pSummoned->CastSpell(pSummoned, SPELL_GREEN_SKYBEAM, true);
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (pSummoned->GetEntry() == NPC_BRANN_FLYING_MACHINE)
        {
            // spawn the Archmange and eject Brann
            if (Creature* pArchmage = m_creature->SummonCreature(NPC_ARCHMANGE_RHYDIAN, 235.5596f, -136.1876f, 409.6508f, 1.78f, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                pArchmage->SetWalk(false);
                pArchmage->GetMotionMaster()->MovePoint(1, 239.3158f, -123.6443f, 409.8174f);
            }

            pSummoned->RemoveAllAuras();
        }
        else if (pSummoned->GetEntry() == NPC_ARCHMANGE_RHYDIAN)
        {
            if (Creature* pBrann = GetClosestCreatureWithEntry(pSummoned, NPC_BRANN_BRONZEBEARD_LEVIATHAN, 30.0f))
            {
                // rest will be handled by DB scripts
                pBrann->SetWalk(false);
                pBrann->GetMotionMaster()->MoveWaypoint();
            }
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // set boss in combat (if not already)
        m_creature->SetInCombatWithZone();
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget->IsVehicle() && pSpellEntry->Id == SPELL_PURSUED)
        {
            m_creature->FixateTarget(pTarget);

            if (Player* pPlayer = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())
                DoScriptText(EMOTE_PURSUE, m_creature, pPlayer);
        }
    }

    // check for all active towers
    void FetchTowers()
    {
        if (!m_pInstance)
            return;

        // the orbital support applies the tower auras
        Creature* pOrbital = m_pInstance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT);
        if (!pOrbital)
            return;

        uint8 uiActiveTowers = 0;

        // check the states twice: at reset and at aggro to make sure that some towers were not destroyed in the meanwhile
        if (m_pInstance->GetData(TYPE_TOWER_HODIR) == DONE)
        {
            pOrbital->CastSpell(pOrbital, SPELL_TOWER_OF_FROST, true);
            ++uiActiveTowers;
            m_bUlduarTower[TOWER_ID_HODIR] = true;
        }
        else
            pOrbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_FROST);
        if (m_pInstance->GetData(TYPE_TOWER_FREYA) == DONE)
        {
            pOrbital->CastSpell(pOrbital, SPELL_TOWER_OF_LIFE, true);
            ++uiActiveTowers;
            m_bUlduarTower[TOWER_ID_FREYA] = true;
        }
        else
            pOrbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_LIFE);
        if (m_pInstance->GetData(TYPE_TOWER_MIMIRON) == DONE)
        {
            pOrbital->CastSpell(pOrbital, SPELL_TOWER_OF_FLAMES, true);
            ++uiActiveTowers;
            m_bUlduarTower[TOWER_ID_MIMIRON] = true;
        }
        else
            pOrbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_FLAMES);
        if (m_pInstance->GetData(TYPE_TOWER_THORIM) == DONE)
        {
            pOrbital->CastSpell(pOrbital, SPELL_TOWER_OF_STORMS, true);
            ++uiActiveTowers;
            m_bUlduarTower[TOWER_ID_THORIM] = true;
        }
        else
            pOrbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_STORMS);

        // inform instance about all active towers for future use in achievements and hard mode loot
        m_pInstance->SetData(TYPE_LEVIATHAN_HARD, uiActiveTowers);
    }

    // Functions which handle the spawn of each type of add
    void DoSpawnHodirFury()
    {
        for (uint8 i = 0; i < MAX_HODIR_FURY; ++i)
            m_creature->SummonCreature(NPC_HODIR_FURY_VEHICLE, afHodirFury[i][0], afHodirFury[i][1], afHodirFury[i][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void DoSpawnFreyaWard()
    {
        for (uint8 i = 0; i < MAX_FREYA_WARD; ++i)
            m_creature->SummonCreature(NPC_FREYA_WARD_VEHICLE, afFreyaWard[i][0], afFreyaWard[i][1], afFreyaWard[i][2], afFreyaWard[i][3], TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void DoSpawnMimironInferno()
    {
        // Mimiron inferno has waypoint movement
        m_creature->SummonCreature(NPC_MIMIRON_INFERNO_VEHICLE, afMimironInferno[0], afMimironInferno[1], afMimironInferno[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void DoSpawnThorimHammer()
    {
        if (!m_pInstance)
            return;

        // get a random point compared to the center and spawn the npcs
        if (Creature* pOrbital = m_pInstance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(pOrbital->GetPositionX(), pOrbital->GetPositionY(), pOrbital->GetPositionZ(), 150.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_THORIM_HAMMER_VEHICLE, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // fetch all tower buffs on the first update
        if (!m_bInitTowers)
        {
            FetchTowers();
            m_bInitTowers = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPursueTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PURSUED) == CAST_OK)
            {
                // don't yell from the beginning
                if (!m_uiHardModeTimer)
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_CHANGE_1, m_creature); break;
                        case 1: DoScriptText(SAY_CHANGE_2, m_creature); break;
                        case 2: DoScriptText(SAY_CHANGE_3, m_creature); break;
                    }
                }
                m_uiPursueTimer = 30000;
            }
        }
        else
            m_uiPursueTimer -= uiDiff;

        if (m_uiFlameVentsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FLAME_VENTS) == CAST_OK)
                m_uiFlameVentsTimer = urand(15000, 25000);
        }
        else
            m_uiFlameVentsTimer -= uiDiff;

        if (m_uiBatteringRamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BATTERING_RAM) == CAST_OK)
                m_uiBatteringRamTimer = urand(10000, 15000);
        }
        else
            m_uiBatteringRamTimer -= uiDiff;

        if (m_uiMissileBarrageTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MISSILE_BARRAGE_TARGET) == CAST_OK)
                    m_uiMissileBarrageTimer = urand(1000, 2000);
            }
        }
        else
            m_uiMissileBarrageTimer -= uiDiff;

        if (m_uiGatheringSpeedTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GATHERING_SPEED) == CAST_OK)
                m_uiGatheringSpeedTimer = 10000;
        }
        else
            m_uiGatheringSpeedTimer -= uiDiff;

        // start hard mode
        if (m_uiHardModeTimer)
        {
            if (m_uiHardModeTimer <= uiDiff)
            {
                // if all towers are deactivated then skip the rest
                if (!m_bUlduarTower[TOWER_ID_HODIR] && !m_bUlduarTower[TOWER_ID_FREYA] && !m_bUlduarTower[TOWER_ID_MIMIRON] && !m_bUlduarTower[TOWER_ID_THORIM])
                {
                    DoScriptText(SAY_TOWER_DOWN, m_creature);
                    m_uiHardModeTimer = 0;
                }
                else
                {
                    // yell hard mode start and start activating each tower one by one
                    switch (m_uiHardModeStep)
                    {
                        case 0:
                            DoScriptText(SAY_HARD_MODE, m_creature);
                            m_uiHardModeTimer = 10000;
                            ++m_uiHardModeStep;
                            break;
                        default:
                            // iterate through all towers to check which is active; skip the ones which are deactivated without triggering the timer
                            for (uint8 i = m_uiHardModeStep - 1; i < KEEPER_ENCOUNTER; ++i)
                            {
                                if (m_bUlduarTower[i])
                                {
                                    // yell tower active
                                    DoScriptText(aLeviathanTowerYell[i], m_creature);
                                    DoScriptText(aLeviathanTowerEmote[i], m_creature);

                                    // activate the timer for each tower ability
                                    switch (i)
                                    {
                                        case TOWER_ID_HODIR:    DoSpawnHodirFury();           break;
                                        case TOWER_ID_FREYA:    DoSpawnFreyaWard();           break;
                                        case TOWER_ID_MIMIRON:  DoSpawnMimironInferno();      break;
                                        case TOWER_ID_THORIM:   m_uiThorimHammerTimer = 1000; break;
                                    }

                                    // reset timer and wait for another turn
                                    m_uiHardModeTimer = 10000;
                                    ++m_uiHardModeStep;
                                    break;
                                }
                                else
                                    ++m_uiHardModeStep;

                                // stop the timer after the final element
                                if (i == KEEPER_ENCOUNTER - 1)
                                    m_uiHardModeTimer = 0;
                            }
                            break;
                    }
                }
            }
            else
                m_uiHardModeTimer -= uiDiff;
        }

        // Tower of Storm abilities
        if (m_uiThorimHammerTimer)
        {
            if (m_uiThorimHammerTimer <= uiDiff)
            {
                DoSpawnThorimHammer();
                ++m_uiThorimHammerCount;

                if (m_uiThorimHammerCount == MAX_THORIM_HAMMER)
                    m_uiThorimHammerTimer = 0;
                else
                    m_uiThorimHammerTimer = 1000;
            }
            else
                m_uiThorimHammerTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_flame_leviathan(Creature* pCreature)
{
    return new boss_flame_leviathanAI(pCreature);
}

/*######
## npc_hodir_fury_reticle
######*/

struct npc_hodir_fury_reticleAI : public ScriptedAI
{
    npc_hodir_fury_reticleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiTargetChaseTimer;
    ObjectGuid m_hodirFuryGuid;

    void Reset() override
    {
        m_uiTargetChaseTimer = 5000;
        SetCombatMovement(false);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HODIR_FURY)
            m_hodirFuryGuid = pSummoned->GetObjectGuid();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // cast Hodir Fury on point reached and search for another target
        if (Creature* pHodirFury = m_creature->GetMap()->GetCreature(m_hodirFuryGuid))
            pHodirFury->CastSpell(m_creature, SPELL_HODIR_FURY, true);

        m_uiTargetChaseTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTargetChaseTimer)
        {
            if (m_uiTargetChaseTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN))
                    {
                        if (Unit* pTarget = pLeviathan->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            m_creature->GetMotionMaster()->MovePoint(1, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
                    }
                }
                m_uiTargetChaseTimer = 0;
            }
            else
                m_uiTargetChaseTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_hodir_fury_reticle(Creature* pCreature)
{
    return new npc_hodir_fury_reticleAI(pCreature);
}

/*######
## npc_hodir_fury
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_hodir_furyAI : public Scripted_NoMovementAI
{
    npc_hodir_furyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_hodir_fury(Creature* pCreature)
{
    return new npc_hodir_furyAI(pCreature);
}

/*######
## npc_freya_ward
######*/

// TODO Move this 'script' to eventAI when combat can be proper prevented from core-side
struct npc_freya_wardAI : public Scripted_NoMovementAI
{
    npc_freya_wardAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset();}

    uint32 m_uiFreyaWardTimer;

    void Reset() override
    {
        m_uiFreyaWardTimer = 30000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_WRITHING_LASHER || pSummoned->GetEntry() == NPC_WARD_OF_LIFE)
            pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiFreyaWardTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FREYA_WARD) == CAST_OK)
                m_uiFreyaWardTimer = 30000;
        }
        else
            m_uiFreyaWardTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_freya_ward(Creature* pCreature)
{
    return new npc_freya_wardAI(pCreature);
}

/*######
## npc_mimiron_inferno
######*/

// TODO Move this 'script' to eventAI when combat can be proper prevented from core-side
struct npc_mimiron_infernoAI : public Scripted_NoMovementAI
{
    npc_mimiron_infernoAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiMimironInfernoTimer;

    void Reset() override
    {
        m_uiMimironInfernoTimer = 15000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMimironInfernoTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MIMIRON_INFERNO) == CAST_OK)
                m_uiMimironInfernoTimer = 1000;
        }
        else
            m_uiMimironInfernoTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mimiron_inferno(Creature* pCreature)
{
    return new npc_mimiron_infernoAI(pCreature);
}

void AddSC_boss_flame_leviathan()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_flame_leviathan";
    pNewScript->GetAI = GetAI_boss_flame_leviathan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hodir_fury_reticle";
    pNewScript->GetAI = GetAI_npc_hodir_fury_reticle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hodir_fury";
    pNewScript->GetAI = GetAI_npc_hodir_fury;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_freya_ward";
    pNewScript->GetAI = GetAI_npc_freya_ward;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mimiron_inferno";
    pNewScript->GetAI = GetAI_npc_mimiron_inferno;
    pNewScript->RegisterSelf();
}
