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

#include "AI/ScriptDevAI/base/TimerAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

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
    NPC_LEVIATHAN_SEAT                      = 33114,
    NPC_LEVIATHAN_TURRET                    = 33139,
    NPC_DEFENSE_TURRET                      = 33142,
    NPC_OVERLOAD_DEVICE                     = 33143,

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

static const int32 leviathanTowerYell[KEEPER_ENCOUNTER] = { SAY_TOWER_FROST, SAY_TOWER_NATURE, SAY_TOWER_FIRE, SAY_TOWER_ENERGY };
static const int32 leviathanTowerEmote[KEEPER_ENCOUNTER] = { EMOTE_HODIR_FURY, EMOTE_FREYA_WARD, EMOTE_MIMIRON_INFERNO, EMOTE_THORIM_HAMMER };

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

enum FlameLeviathanActions
{
    LEVIATHAN_FETCH_TOWERS,
    LEVIATHAN_HARDMODES,
    LEVIATHAN_THORIMS_HAMMER,
    LEVIATHAN_ACTIONS_MAX,
};

struct boss_flame_leviathanAI : public BossAI
{
    boss_flame_leviathanAI(Creature* creature) : BossAI(creature, LEVIATHAN_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_LEVIATHAN);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY);
        AddOnDeathText(SAY_DEATH);
        AddRespawnOnEvade(30s);
        AddCustomAction(LEVIATHAN_THORIMS_HAMMER, true, [&]()
        {
            DoSpawnThorimsHammer();
            ++m_thorimsHammerCount;

            if (m_thorimsHammerCount < MAX_THORIM_HAMMER)
                ResetTimer(LEVIATHAN_THORIMS_HAMMER, 1s);
        });
        AddTimerlessCombatAction(LEVIATHAN_FETCH_TOWERS, true);
        AddCombatAction(LEVIATHAN_HARDMODES, 10s);
        Reset();
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;
    bool m_initTowers;
    bool m_ulduarTower[KEEPER_ENCOUNTER];

    uint8 m_hardmodeStep;

    uint8 m_thorimsHammerCount;
    uint32 m_uiThorimHammerTimer;

    void Reset() override
    {
        m_initTowers = false;
        DoCastSpellIfCan(m_creature, SPELL_INVISIBILITY_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        for (bool& i : m_ulduarTower)
            i = false;

        m_hardmodeStep         = 0;
        m_thorimsHammerCount   = 0;
    }

    void JustDied(Unit* /*killer*/) override
    {
        BossAI::JustDied();
        if (m_instance)
        {
            // clear hard mode auras
            if (Creature* orbital = m_instance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT))
                orbital->RemoveAllAuras();
        }

        // start epilogue event
        if (Creature* pFlyMachine = m_creature->SummonCreature(NPC_BRANN_FLYING_MACHINE, 175.2838f, -210.4325f, 501.2375f, 1.42f, TEMPSPAWN_CORPSE_DESPAWN, 0))
        {
            if (Creature* brann = m_creature->SummonCreature(NPC_BRANN_BRONZEBEARD_LEVIATHAN, 175.2554f, -210.6305f, 500.7375f, 1.42f, TEMPSPAWN_CORPSE_DESPAWN, 0))
                brann->CastSpell(pFlyMachine, SPELL_RIDE_VEHICLE, TRIGGERED_OLD_TRIGGERED);

            pFlyMachine->SetWalk(false);
            pFlyMachine->GetMotionMaster()->MovePoint(1, 229.9419f, -130.3764f, 409.5681f);
        }
    }

    void EnterEvadeMode() override
    {
        static const std::vector<uint32> addEntries = {NPC_LEVIATHAN_SEAT, NPC_LEVIATHAN_TURRET, NPC_DEFENSE_TURRET, NPC_OVERLOAD_DEVICE};
        CreatureList leviAdds;
        for (const uint32& entry : addEntries)
        {
            GetCreatureListWithEntryInGrid(leviAdds, m_creature, entry, 50.f);
            for (auto& add : leviAdds)
            {
                add->ForcedDespawn();
            }
        }
        BossAI::EnterEvadeMode();
    }

    void JustSummoned(Creature* summoned) override
    {
        m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
        switch (summoned->GetEntry())
        {
            case NPC_THORIM_HAMMER_VEHICLE:
                summoned->CastSpell(summoned, SPELL_LIGHTNING_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_MIMIRON_INFERNO_VEHICLE:
                summoned->CastSpell(summoned, SPELL_RED_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_HODIR_FURY_VEHICLE:
                summoned->CastSpell(summoned, SPELL_BLUE_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_FREYA_WARD_VEHICLE:
                summoned->CastSpell(summoned, SPELL_GREEN_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId)
            return;

        if (summoned->GetEntry() == NPC_BRANN_FLYING_MACHINE)
        {
            // spawn the Archmange and eject Brann
            if (Creature* archmage = m_creature->SummonCreature(NPC_ARCHMANGE_RHYDIAN, 235.5596f, -136.1876f, 409.6508f, 1.78f, TEMPSPAWN_CORPSE_DESPAWN, 0))
            {
                archmage->SetWalk(false);
                archmage->GetMotionMaster()->MovePoint(1, 239.3158f, -123.6443f, 409.8174f);
            }

            summoned->RemoveAllAuras();
        }
        else if (summoned->GetEntry() == NPC_ARCHMANGE_RHYDIAN)
        {
            if (Creature* brann = GetClosestCreatureWithEntry(summoned, NPC_BRANN_BRONZEBEARD_LEVIATHAN, 30.0f))
            {
                // rest will be handled by DB scripts
                brann->SetWalk(false);
                brann->GetMotionMaster()->MoveWaypoint();
            }
        }
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != EFFECT_MOTION_TYPE)
            return;

        // set boss in combat (if not already)
        m_creature->SetInCombatWithZone();
    }

    void HandleHardmode()
    {
        // if all towers are deactivated then skip the rest
        if (!m_ulduarTower[TOWER_ID_HODIR] && !m_ulduarTower[TOWER_ID_FREYA] && !m_ulduarTower[TOWER_ID_MIMIRON] && !m_ulduarTower[TOWER_ID_THORIM])
        {
            DoBroadcastText(SAY_TOWER_DOWN, m_creature);
            DisableCombatAction(LEVIATHAN_HARDMODES);
        }
        else
        {
            // yell hard mode start and start activating each tower one by one
            switch (m_hardmodeStep)
            {
                case 0:
                    DoBroadcastText(SAY_HARD_MODE, m_creature);
                    ResetCombatAction(LEVIATHAN_HARDMODES, 10s);
                    ++m_hardmodeStep;
                    break;
                default:
                    // iterate through all towers to check which is active; skip the ones which are deactivated without triggering the timer
                    for (uint8 i = m_hardmodeStep - 1; i < KEEPER_ENCOUNTER; ++i)
                    {
                        if (m_ulduarTower[i])
                        {
                            // yell tower active
                            DoBroadcastText(leviathanTowerYell[i], m_creature);
                            DoBroadcastText(leviathanTowerEmote[i], m_creature);

                            // activate the timer for each tower ability
                            switch (i)
                            {
                                case TOWER_ID_HODIR:    DoSpawnHodirsFury();           break;
                                case TOWER_ID_FREYA:    DoSpawnFreyasWard();           break;
                                case TOWER_ID_MIMIRON:  DoSpawnMimironsInferno();      break;
                                case TOWER_ID_THORIM:   ResetTimer(LEVIATHAN_THORIMS_HAMMER, 1s); break;
                            }

                            // reset timer and wait for another turn
                            ResetCombatAction(LEVIATHAN_HARDMODES, 10s);
                            ++m_hardmodeStep;
                            break;
                        }
                        ++m_hardmodeStep;

                        // stop the timer after the final element
                        if (i == KEEPER_ENCOUNTER - 1)
                            DisableCombatAction(LEVIATHAN_HARDMODES);
                    }
                    break;
            }
        }
    }

    // check for all active towers
    void FetchTowers()
    {
        if (!m_instance)
            return;

        // the orbital support applies the tower auras
        Creature* orbital = m_instance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT);
        if (!orbital)
            return;

        uint8 activeTowers = 0;

        // check the states twice: at reset and at aggro to make sure that some towers were not destroyed in the meanwhile
        if (m_instance->GetData(TYPE_TOWER_HODIR) == DONE)
        {
            orbital->CastSpell(orbital, SPELL_TOWER_OF_FROST, TRIGGERED_OLD_TRIGGERED);
            ++activeTowers;
            m_ulduarTower[TOWER_ID_HODIR] = true;
        }
        else
            orbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_FROST);
        if (m_instance->GetData(TYPE_TOWER_FREYA) == DONE)
        {
            orbital->CastSpell(orbital, SPELL_TOWER_OF_LIFE, TRIGGERED_OLD_TRIGGERED);
            ++activeTowers;
            m_ulduarTower[TOWER_ID_FREYA] = true;
        }
        else
            orbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_LIFE);
        if (m_instance->GetData(TYPE_TOWER_MIMIRON) == DONE)
        {
            orbital->CastSpell(orbital, SPELL_TOWER_OF_FLAMES, TRIGGERED_OLD_TRIGGERED);
            ++activeTowers;
            m_ulduarTower[TOWER_ID_MIMIRON] = true;
        }
        else
            orbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_FLAMES);
        if (m_instance->GetData(TYPE_TOWER_THORIM) == DONE)
        {
            orbital->CastSpell(orbital, SPELL_TOWER_OF_STORMS, TRIGGERED_OLD_TRIGGERED);
            ++activeTowers;
            m_ulduarTower[TOWER_ID_THORIM] = true;
        }
        else
            orbital->RemoveAurasDueToSpell(SPELL_TOWER_OF_STORMS);

        // inform instance about all active towers for future use in achievements and hard mode loot
        m_instance->SetData(TYPE_LEVIATHAN_HARD, activeTowers);
    }

    // Functions which handle the spawn of each type of add
    void DoSpawnHodirsFury()
    {
        for (auto i : afHodirFury)
            m_creature->SummonCreature(NPC_HODIR_FURY_VEHICLE, i[0], i[1], i[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void DoSpawnFreyasWard()
    {
        for (auto i : afFreyaWard)
            m_creature->SummonCreature(NPC_FREYA_WARD_VEHICLE, i[0], i[1], i[2], i[3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void DoSpawnMimironsInferno()
    {
        // Mimiron inferno has waypoint movement
        m_creature->SummonCreature(NPC_MIMIRON_INFERNO_VEHICLE, afMimironInferno[0], afMimironInferno[1], afMimironInferno[2], 0, TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    void DoSpawnThorimsHammer()
    {
        if (!m_instance)
            return;

        // get a random point compared to the center and spawn the npcs
        if (Creature* orbital = m_instance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(orbital->GetPositionX(), orbital->GetPositionY(), orbital->GetPositionZ(), 150.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_THORIM_HAMMER_VEHICLE, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 8000);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LEVIATHAN_FETCH_TOWERS:
            {
                FetchTowers();
                DisableCombatAction(action);
                return;
            }
            case LEVIATHAN_HARDMODES:
            {
                HandleHardmode();
            }
        }
    }
};

/*######
## npc_hodir_fury_reticle
######*/

struct npc_hodir_fury_reticleAI : public ScriptedAI
{
    npc_hodir_fury_reticleAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = (ScriptedInstance*)creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiTargetChaseTimer;
    ObjectGuid m_hodirFuryGuid;

    void Reset() override
    {
        m_uiTargetChaseTimer = 5000;
        SetCombatMovement(false);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_HODIR_FURY)
            m_hodirFuryGuid = summoned->GetObjectGuid();
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId)
            return;

        // cast Hodir Fury on point reached and search for another target
        if (Creature* pHodirFury = m_creature->GetMap()->GetCreature(m_hodirFuryGuid))
            pHodirFury->CastSpell(m_creature, SPELL_HODIR_FURY, TRIGGERED_OLD_TRIGGERED);

        m_uiTargetChaseTimer = 5000;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiTargetChaseTimer)
        {
            if (m_uiTargetChaseTimer <= diff)
            {
                if (m_instance)
                {
                    if (Creature* pLeviathan = m_instance->GetSingleCreatureFromStorage(NPC_LEVIATHAN))
                    {
                        if (Unit* target = pLeviathan->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            m_creature->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                    }
                }
                m_uiTargetChaseTimer = 0;
            }
            else
                m_uiTargetChaseTimer -= diff;
        }
    }
};

/*######
## npc_hodir_fury
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_hodir_furyAI : public Scripted_NoMovementAI
{
    npc_hodir_furyAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(const uint32 /*diff*/) override { }
};

/*######
## npc_freya_ward
######*/

// TODO Move this 'script' to eventAI when combat can be proper prevented from core-side
struct npc_freya_wardAI : public Scripted_NoMovementAI
{
    npc_freya_wardAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset();}

    uint32 m_uiFreyaWardTimer;

    void Reset() override
    {
        m_uiFreyaWardTimer = 30000;
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_WRITHING_LASHER || summoned->GetEntry() == NPC_WARD_OF_LIFE)
            summoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiFreyaWardTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FREYA_WARD) == CAST_OK)
                m_uiFreyaWardTimer = 30000;
        }
        else
            m_uiFreyaWardTimer -= diff;
    }
};

/*######
## npc_mimiron_inferno
######*/

// TODO Move this 'script' to eventAI when combat can be proper prevented from core-side
struct npc_mimiron_infernoAI : public Scripted_NoMovementAI
{
    npc_mimiron_infernoAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    uint32 m_uiMimironInfernoTimer;

    void Reset() override
    {
        m_uiMimironInfernoTimer = 15000;
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiMimironInfernoTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MIMIRON_INFERNO) == CAST_OK)
                m_uiMimironInfernoTimer = 1000;
        }
        else
            m_uiMimironInfernoTimer -= diff;
    }
};

struct PursueLeviathan : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* caster = spell->GetCaster();
        if (!caster || !caster->AI() || caster->GetEntry() != NPC_LEVIATHAN)
            return;
        if (static_cast<boss_flame_leviathanAI*>(caster->AI())->TimeSinceEncounterStart() < 10s)
            return;
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_CHANGE_1, caster); break;
            case 1: DoBroadcastText(SAY_CHANGE_2, caster); break;
            case 2: DoBroadcastText(SAY_CHANGE_3, caster); break;
        }
    }

    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target)
            return;
        if (!target->IsVehicle())
            return;
        caster->FixateTarget(target);
        if (Player* player = target->GetBeneficiaryPlayer())
            DoBroadcastText(EMOTE_PURSUE, caster, player);
    }
};

struct BatteringRamLeviathan : public SpellScript
{
    // TODO: Figure out Targeting issues
};

// 62297 Hodir's Fury
struct HodirsFuryLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return false;
        if (caster->IsFriend(target))
            return false;
        if (!target->IsPlayer() && !target->IsControlledByPlayer())
            return false;
        return true;
    }
};

// 62912 Thorim's Hammer
struct ThorimsHammerLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return false;
        if (caster->IsFriend(target))
            return false;
        if (!target->IsPlayer() && !target->IsControlledByPlayer())
            return false;
        return true;
    }
};

// 62910 Mimiron's Inferno
struct MimironsInfernoLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return false;
        if (caster->IsFriend(target))
            return false;
        if (!target->IsPlayer() && !target->IsControlledByPlayer())
            return false;
        return true;
    }
};

void AddSC_boss_flame_leviathan()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_flame_leviathan";
    pNewScript->GetAI = &GetNewAIInstance<boss_flame_leviathanAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hodir_fury_reticle";
    pNewScript->GetAI = &GetNewAIInstance<npc_hodir_fury_reticleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hodir_fury";
    pNewScript->GetAI = &GetNewAIInstance<npc_hodir_furyAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_freya_ward";
    pNewScript->GetAI = &GetNewAIInstance<npc_freya_wardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mimiron_inferno";
    pNewScript->GetAI = &GetNewAIInstance<npc_mimiron_infernoAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PursueLeviathan>("spell_pursue_leviathan");
    RegisterSpellScript<HodirsFuryLeviathan>("spell_hodirs_fury_leviathan");
    RegisterSpellScript<ThorimsHammerLeviathan>("spell_thorims_hammer_leviathan");
    RegisterSpellScript<MimironsInfernoLeviathan>("spell_mimirons_inferno_leviathan");
}
