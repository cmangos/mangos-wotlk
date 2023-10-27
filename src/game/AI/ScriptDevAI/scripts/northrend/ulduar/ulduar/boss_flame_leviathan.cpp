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
SD%Complete: 100%
SDComment: 
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Maps/GridDefines.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellEffectDefines.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "Entities/Vehicle.h"

enum
{
    SAY_AGGRO                               = 33487,
    SAY_SLAY                                = 33507,
    SAY_DEATH                               = 33506,
    SAY_CHANGE_1                            = 33488,
    SAY_CHANGE_2                            = 33489,
    SAY_CHANGE_3                            = 33490,

    SAY_PLAYER_RIDE                         = 33501,
    SAY_OVERLOAD_1                          = 33503,
    SAY_OVERLOAD_2                          = 33504,
    SAY_OVERLOAD_3                          = 33505,

    SAY_HARD_MODE                           = 33491,
    SAY_TOWER_FROST                         = 33493,
    SAY_TOWER_FIRE                          = 33495,
    SAY_TOWER_ENERGY                        = 33499,
    SAY_TOWER_NATURE                        = 33497,
    SAY_TOWER_DOWN                          = 33492,

    EMOTE_PURSUE                            = 33502,
    EMOTE_HODIR_FURY                        = 33494,
    EMOTE_FREYA_WARD                        = 33498,
    EMOTE_MIMIRON_INFERNO                   = 33496,
    EMOTE_THORIM_HAMMER                     = 33500,

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

    // leviathan other spells
    SPELL_SMOKE_TRAIL                       = 63575,
    SPELL_EJECT_ALL_PASSENGERS              = 50630,                    // used by vehicles on death; currently handled by DB linking
    SPELL_EJECT_PASSENGER_4                 = 64614,
    SPELL_EJECT_PASSENGER_1                 = 60603,
    SPELL_LOAD_INTO_CATAPULT                = 64414,
    SPELL_PASSENGER_LOADED                  = 62340,
    SPELL_HOOKSHOT_AURA                     = 62336,
    SPELL_RIDE_VEHICLE_SCALES_WITH_GEAR     = 62309,
    SPELL_OVERLOAD                          = 63618,

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
    SPELL_BEAM_TARGET_STATE                 = 62898,                    // cast by all tower reticles; purpose unk
    SPELL_BIRTH                             = 40031,                    // not used; purpose unk

    // player vehicle spells
    SPELL_LIQUID_PYRITE_AURA                = 62494,
    SPELL_LIQUID_PYRITE                     = 62496,
    SPELL_RELOAD_AMMMO                      = 62473,
    SPELL_GENERAL_TRIGGER_1_FROM_PSG_2      = 69748,
    SPELL_GENERAL_TRIGGER_2_TO_SELF         = 67395,
    SPELL_GRAB_PYRITE                       = 67372,
    SPELL_EJECT_PASSENGER                   = 67393,

    // gauntlet spells
    SPELL_DUSTY_EXPLOSION                   = 63360,
    SPELL_DUST_CLOUD_IMPACT                 = 54740,
    SPELL_SPAWN_PYRITE                      = 62543,
    SPELL_COSMETIC_PARACHITE                = 56093,
    SPELL_ROPE_BEAM                         = 63605,

    // vehicle accessories
    //NPC_LEVIATHAN_SEAT                      = 33114,
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

    /*
UPDATE vehicle_accessory SET seat=0 WHERE vehicle_entry IN (33364, 33369, 33108, 33366);
    */

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
    {156.9291f, 61.52306f, 410.887f, 5.68f},
    {376.641f, 68.61361f, 412.2287f, 3.85f},
    {383.6206f, -130.8576f, 411.7088f, 2.26f},
    {154.9095f, -137.4339f, 410.887f, 0.79f},
};

static const float afHodirFury[MAX_HODIR_FURY][3] =
{
    {219.9013f, 7.913357f, 410.7861f},
    {326.0777f, -74.99034f, 410.887f},
};

static const float afMimironInferno[3] = {329.1809f, 8.02577f, 410.887f};

static const std::vector<uint32> addEntries = {NPC_LEVIATHAN_SEAT, NPC_LEVIATHAN_TURRET, NPC_DEFENSE_TURRET, NPC_OVERLOAD_DEVICE};
static const std::vector<uint32> playerVehicleEntries = {NPC_SALVAGED_SIEGE_ENGINE, NPC_SALVAGED_SIEGE_TURRET, NPC_SALVAGED_CHOPPER, NPC_SALVAGED_DEMOLISHER, NPC_SALVAGED_DEMOLISHER_SEAT};

/*######
## boss_flame_leviathan
######*/

enum FlameLeviathanActions
{
    LEVIATHAN_FETCH_TOWERS,
    LEVIATHAN_HARDMODES,
    LEVIATHAN_THORIMS_HAMMER,
    LEVIATHAN_RESET_OVERLOAD,
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
        AddTimerlessCombatAction(LEVIATHAN_FETCH_TOWERS, true);
        AddCombatAction(LEVIATHAN_HARDMODES, 10s);
        AddCombatAction(LEVIATHAN_THORIMS_HAMMER, true);
        AddEntranceObject(GO_SHIELD_WALL);
        AddExitObject(GO_LIGHTNING_DOOR, GO_XT002_GATE);
        SetGateDelay(5s);
        Reset();
        AddCustomAction(LEVIATHAN_RESET_OVERLOAD, true, [&]()
        {
            Aura* aura = m_creature->GetAura(SPELL_OVERLOAD_CIRCUIT, EFFECT_INDEX_1);
            if (!aura)
                return;
            if (Unit* caster = aura->GetCaster())
                caster->InterruptNonMeleeSpells(true);
        });
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float, float)
        {
            return x < 148;
        });
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
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

        CreatureList leviAdds;
        for (const uint32& entry : addEntries)
        {
            GetCreatureListWithEntryInGrid(leviAdds, m_creature, entry, 50.f);
            for (auto add : leviAdds)
            {
                if (add && (add->GetEntry() == NPC_DEFENSE_TURRET || add->GetEntry() == NPC_LEVIATHAN_TURRET || add->GetEntry() == NPC_LEVIATHAN_SEAT))
                    add->Suicide();
                else if (add)
                    add->ForcedDespawn();
            }
        }

        CreatureList playerVehicles;
        for (const uint32& entry : playerVehicleEntries)
        {
            GetCreatureListWithEntryInGrid(playerVehicles, m_creature, entry, 1000.f); // probably very expensive. better solution?
            for (auto* add : playerVehicles)
            {
                if (!add || !add->IsAlive())
                    continue;
                switch (add->GetEntry())
                {
                    case NPC_SALVAGED_SIEGE_ENGINE:
                    case NPC_SALVAGED_CHOPPER:
                        add->CastSpell(nullptr, SPELL_EJECT_PASSENGER_1, TRIGGERED_OLD_TRIGGERED);
                        break;
                    case NPC_SALVAGED_DEMOLISHER:
                        add->CastSpell(nullptr, SPELL_EJECT_PASSENGER_4, TRIGGERED_OLD_TRIGGERED);
                        break;
                    case NPC_SALVAGED_SIEGE_TURRET:
                    case NPC_SALVAGED_DEMOLISHER_SEAT:
                        add->CastSpell(nullptr, SPELL_EJECT_ALL_PASSENGERS, TRIGGERED_OLD_TRIGGERED);
                        break;
                    default: break;
                }
            }
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

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);
        SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_A, nullptr, 0, 20.f);
    }

    void EnterEvadeMode() override
    {
        m_creature->CastSpell(m_creature, SPELL_EJECT_ALL_PASSENGERS, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_OVERLOAD, TRIGGERED_OLD_TRIGGERED);
        CreatureList leviAdds;
        for (const uint32& entry : addEntries)
        {
            GetCreatureListWithEntryInGrid(leviAdds, m_creature, entry, 50.f);
            for (auto add : leviAdds)
            {
                if (add)
                    add->ForcedDespawn();
            }
        }

        if (m_instance)
        {
            // clear hard mode auras
            if (Creature* orbital = m_instance->GetSingleCreatureFromStorage(NPC_ORBITAL_SUPPORT))
                orbital->RemoveAllAuras();
        }
        BossAI::EnterEvadeMode();
        m_creature->ForcedDespawn(3000);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_THORIM_HAMMER_VEHICLE:
                summoned->CastSpell(summoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_BEAM_TARGET_STATE, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, 50195, TRIGGERED_OLD_TRIGGERED); // Hackfix for reticles until hovering is properly implemented
                summoned->CastSpell(summoned, SPELL_LIGHTNING_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
                break;
            case NPC_MIMIRON_INFERNO_VEHICLE:
                summoned->CastSpell(summoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_BEAM_TARGET_STATE, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, 50195, TRIGGERED_OLD_TRIGGERED); // Hackfix for reticles until hovering is properly implemented
                summoned->CastSpell(summoned, SPELL_RED_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
                break;
            case NPC_HODIR_FURY_VEHICLE:
                summoned->CastSpell(summoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_BEAM_TARGET_STATE, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, 50195, TRIGGERED_OLD_TRIGGERED); // Hackfix for reticles until hovering is properly implemented
                summoned->CastSpell(summoned, SPELL_BLUE_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
                break;
            case NPC_FREYA_WARD_VEHICLE:
                summoned->CastSpell(summoned, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_BEAM_TARGET_STATE, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, 50195, TRIGGERED_OLD_TRIGGERED); // Hackfix for reticles until hovering is properly implemented
                summoned->CastSpell(summoned, SPELL_GREEN_SKYBEAM, TRIGGERED_OLD_TRIGGERED);
                m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
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
        m_creature->SetInCombatWithZone(false);
    }

    void HandleHardmode()
    {
        if(!m_creature->IsInCombat())
        {
            DisableCombatAction(LEVIATHAN_HARDMODES);
            return;
        }
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
                                case TOWER_ID_THORIM:   ResetCombatAction(LEVIATHAN_THORIMS_HAMMER, 1s); break;
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
            m_creature->SummonCreature(NPC_THORIM_HAMMER_VEHICLE, fX, fY, fZ + 1, 0, TEMPSPAWN_TIMED_DESPAWN, 8000);
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
            case LEVIATHAN_THORIMS_HAMMER:
            {
                DoSpawnThorimsHammer();
                ++m_thorimsHammerCount;

                if (m_thorimsHammerCount < MAX_THORIM_HAMMER)
                    ResetCombatAction(LEVIATHAN_THORIMS_HAMMER, 500ms);
                else
                {
                    ResetCombatAction(action, 80s);
                    m_thorimsHammerCount = 0;
                }
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
    std::chrono::time_point<std::chrono::steady_clock> m_castOnCooldown = std::chrono::steady_clock::now();
    ObjectGuid m_hodirFuryGuid;

    void Reset() override
    {
        m_uiTargetChaseTimer = 5000;
        SetCombatMovement(false);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetDistance2d(m_creature->GetPositionX(), m_creature->GetPositionY()) > 3.f)
            return;
        if (std::chrono::steady_clock::now() < m_castOnCooldown)
            return;
        if (!who->HasCharmer() || m_creature->IsFriend(who))
            return;
        m_creature->GetMotionMaster()->Clear();
        m_uiTargetChaseTimer = 0;
        m_castOnCooldown = std::chrono::steady_clock::now() + 5s;
        MovementInform(POINT_MOTION_TYPE, 1);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_HODIR_FURY)
            m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId)
            return;

        // cast Hodir Fury on point reached and search for another target
        //if (Creature* pHodirFury = m_creature->GetMap()->GetCreature(m_hodirFuryGuid))
        if (m_creature->IsVehicle() && m_creature->GetVehicleInfo())
            if (Unit* hodirsFuryCaster = m_creature->GetVehicleInfo()->GetPassenger(0))
                hodirsFuryCaster->CastSpell(m_creature, SPELL_HODIR_FURY, TRIGGERED_IGNORE_CASTER_AURA_STATE);
        //m_creature->CastSpell(m_creature, SPELL_HODIR_FURY, TRIGGERED_OLD_TRIGGERED);

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
                            m_creature->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 1);
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
            summoned->SetInCombatWithZone(false);
        m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiFreyaWardTimer < diff)
        {
            if (m_creature->IsBoarded())
            {
                Unit* vehicle = dynamic_cast<Unit*>(m_creature->GetTransportInfo()->GetTransport());
                if (vehicle && vehicle->GetVehicleInfo())
                    if (DoCastSpellIfCan(vehicle, SPELL_FREYA_WARD) == CAST_OK)
                        m_uiFreyaWardTimer = 30000;
            }
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
            if (DoCastSpellIfCan(nullptr, SPELL_MIMIRON_INFERNO) == CAST_OK)
                m_uiMimironInfernoTimer = 3000;
        }
        else
            m_uiMimironInfernoTimer -= diff;
    }
};

struct npc_liquid_pyriteAI : public Scripted_NoMovementAI
{
    npc_liquid_pyriteAI(Creature* creature) : Scripted_NoMovementAI(creature) {
        AddCustomAction(0, true, [&]()
        {
            if (!m_creature->IsBoarded())
                return;
            Unit* vehicle = dynamic_cast<Unit*>(m_creature->GetTransportInfo()->GetTransport());
            if (!vehicle)
                return;
            if (vehicle->GetEntry() == 33167 || vehicle->GetEntry() == NPC_SALVAGED_DEMOLISHER)
            {
                m_creature->CastSpell(vehicle, SPELL_LIQUID_PYRITE, TRIGGERED_OLD_TRIGGERED);
                m_creature->ForcedDespawn(3000);
            }
        });
        Reset();
    }

    void Reset() override
    {
        Scripted_NoMovementAI::Reset();
        DoCastSpellIfCan(m_creature, SPELL_LIQUID_PYRITE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void MovementInform(uint32 motionType, uint32 /*value*/) override
    {
        if (motionType != BOARD_VEHICLE_MOTION_TYPE)
            return;
        ResetTimer(0, 2s);
    }
};

struct npc_pyrite_safety_containerAI : public Scripted_NoMovementAI
{
    npc_pyrite_safety_containerAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        AddCustomAction(0, 1s, [&]()
        {
            Creature* lift = dynamic_cast<Creature*>(m_creature->GetSpawner());
            if (!lift)
                return;
            m_creature->CastSpell(lift, SPELL_ROPE_BEAM, TRIGGERED_OLD_TRIGGERED);
        });
        AddCustomAction(1, true, [&]()
        {
            float gZ = m_creature->GetMap()->GetHeight(m_creature->GetPhaseMask(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            if ((gZ + 0.1f) < m_creature->GetPositionZ())
            {
                ResetTimer(1, 500ms);
                return;
            }
            m_creature->CastSpell(nullptr, SPELL_DUSTY_EXPLOSION, TRIGGERED_OLD_TRIGGERED);
            m_creature->CastSpell(nullptr, SPELL_DUST_CLOUD_IMPACT, TRIGGERED_OLD_TRIGGERED);
            m_creature->CastSpell(nullptr, SPELL_SPAWN_PYRITE, TRIGGERED_OLD_TRIGGERED);
            m_creature->ForcedDespawn(1000);
        });
    }
};

enum DemolisherActions
{
    DEMOLISHER_ACTIONS_MAX,
    DEMOLISHER_SYNC_ENERGY,
};

struct npc_salvaged_demolisherAI : public CombatAI
{
    npc_salvaged_demolisherAI(Creature* creature) : CombatAI(creature, DEMOLISHER_ACTIONS_MAX)
    {
        AddCustomAction(DEMOLISHER_SYNC_ENERGY, 1s, [&]() { initialize(); });
        SetCombatMovement(false);
    }

    void initialize()
    {
        m_creature->SetMaxPower(POWER_ENERGY, 50);
        m_creature->SetPower(POWER_ENERGY, 50);
        if (!m_creature->IsVehicle())
            return;
        Unit* mechanicSeat = m_creature->GetVehicleInfo()->GetPassenger(1);
        if (!mechanicSeat)
            return;
        mechanicSeat->SetPowerType(POWER_ENERGY);
        mechanicSeat->SetMaxPower(POWER_ENERGY, 50);
        mechanicSeat->SetPower(POWER_ENERGY, 50);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        ResetTimer(DEMOLISHER_SYNC_ENERGY, 1s);
    }
};

struct npc_salvaged_chopperAI : public CombatAI
{
    npc_salvaged_chopperAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
#ifdef PRENERF_3_4_1
        m_creature->UpdateSpell(3, 0);
#endif
    }

    void OnPassengerRide(Unit* passenger, bool boarded, uint8 seat) override
    {
        Player* driver = dynamic_cast<Player*>(m_creature->GetVehicleInfo()->GetPassenger(0));
        if (!driver)
            return;
        if (!seat)
            return;
        CharmInfo* charmInfo = m_creature->InitCharmInfo(m_creature);
        if (boarded)
        {
            //m_creature->CastSpell(nullptr, SPELL_GENERAL_TRIGGER_1_FROM_PSG_2, TRIGGERED_OLD_TRIGGERED);
            m_creature->UpdateSpell(3, SPELL_EJECT_PASSENGER);
#ifdef PRENERF_3_4_1
            m_creature->UpdateSpell(3, 0);
#endif
            charmInfo->InitVehicleCreateSpells();
            driver->VehicleSpellInitialize();
        }
        else
        {
            m_creature->UpdateSpell(3, SPELL_GRAB_PYRITE);
#ifdef PRENERF_3_4_1
            m_creature->UpdateSpell(3, 0);
#endif
            charmInfo->InitVehicleCreateSpells();
            driver->VehicleSpellInitialize();
        }
    }
};

enum DefenseTurretActions
{
    DEFENSE_TURRET_RANGE_CHECK,
    DEFENSE_TURRET_ACTIONS_MAX,
};

struct npc_leviathan_defense_turretAI : public CombatAI
{
    npc_leviathan_defense_turretAI(Creature* creature) : CombatAI(creature, DEFENSE_TURRET_ACTIONS_MAX)
    {
        SetCombatMovement(false);
        AddCombatAction(DEFENSE_TURRET_RANGE_CHECK, 1s);
    }

    void JustDied(Unit* killer) override
    {
        uint8 seatId = m_creature->GetTransSeat();
        TransportInfo* transportInfo = m_creature->GetTransportInfo();
        if (!transportInfo)
            return;
        Unit* leviSeat = static_cast<Unit*>(transportInfo->GetTransport());
        if (!leviSeat || !leviSeat->IsVehicle())
            return;
        Unit* overloadDevice = leviSeat->GetVehicleInfo()->GetPassenger(seatId == 1 ? 2 : 1);
        if (!overloadDevice)
            return;
        overloadDevice->CastSpell(nullptr, SPELL_OVERLOAD_CIRCUIT, TRIGGERED_OLD_TRIGGERED);
        m_creature->ForcedDespawn(1000);
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == DEFENSE_TURRET_RANGE_CHECK)
        {
            float newHealthVal = m_creature->GetHealthPercent() + 1.f;
            if (!m_creature->GetVictim())
                m_creature->SetHealthPercent(std::min(newHealthVal, 100.f));
            for (auto& attacker : m_creature->getAttackers())
                if (attacker->GetDistance(m_creature, true) >= (20.f * 20.f) || m_creature->IsFriend(attacker))
                    m_creature->getThreatManager().modifyThreatPercent(attacker, -101);
            ResetCombatAction(DEFENSE_TURRET_RANGE_CHECK, 1s);
        }
    }
};

bool NpcSpellClick_npc_salvaged_demolisher(Player* player, Creature* clickedCreature, uint32 spellId)
{
    if (!clickedCreature || clickedCreature->GetEntry() != NPC_SALVAGED_DEMOLISHER)
        return false;
    VehicleInfo* vehicleInfo = clickedCreature->GetVehicleInfo();
    if (!vehicleInfo)
        return false;
    if (vehicleInfo->CanBoard(player))
    {
        player->CastSpell(clickedCreature, SPELL_RIDE_VEHICLE_SCALES_WITH_GEAR, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return true;
    }
    int32 newSeat = 0;
    player->CastCustomSpell(clickedCreature, SPELL_RIDE_VEHICLE_SCALES_WITH_GEAR, &newSeat, nullptr, nullptr, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    return true;
}

struct PursueLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target)
            return false;
        if (!target->IsVehicle())
            return false;
        if (auto vehicleInfo = target->GetVehicleInfo())
            if (vehicleInfo->GetPassenger(0))
                return true;
        return false;
    }

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
        if (!target)
            return true;
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
        if (!target)
            return true;
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
struct MimironsInfernoLeviathan : public SpellScript, public AuraScript
{
    bool OnAreaAuraCheckTarget(DynamicObject* dynGo, Unit* target) const override
    {
        if (!target)
            return false;
        Unit* caster = dynGo->GetCaster();
        if (!caster)
            return false;
        if (caster->IsFriend(target))
        {
            return false;
        }
        if (!target->IsPlayer() && !target->IsControlledByPlayer())
            return false;
        return true;
    }
};

struct LoadIntoCatapultLeviathan : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        if (apply)
        {
            target->CastSpell(nullptr, SPELL_PASSENGER_LOADED, TRIGGERED_OLD_TRIGGERED);
            return;
        }
        target->RemoveAurasDueToSpell(SPELL_PASSENGER_LOADED);
        aura->GetCaster()->RemoveAurasDueToSpell(SPELL_PASSENGER_LOADED);
    }
};

// 62324 Throw Passenger. May also apply to 47792
struct ThrowPassenger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        if (caster->GetEntry() != NPC_SALVAGED_DEMOLISHER)
            return;
        auto& targetList = spell->GetTargetList();
        Unit* projectile = nullptr;
        Unit* seat = nullptr;

        if (caster->GetVehicleInfo())
            if (caster->GetVehicleInfo()->GetPassenger(3))
                if ((seat = static_cast<Unit*>(caster->GetVehicleInfo()->GetPassenger(3))))
                    if (seat->IsVehicle())
                        projectile = seat->GetVehicleInfo()->GetPassenger(0);
        if (!projectile)
            return;
        if (!seat)
            return;

        std::unordered_set<uint32> spellIds;
        for (auto aura : caster->GetAurasByType(SPELL_AURA_CONTROL_VEHICLE))
        {
            if (aura->GetCasterGuid() == projectile->GetObjectGuid())
                spellIds.emplace(aura->GetId());
        }

        for (uint32 spell : spellIds)
            projectile->RemoveAurasByCasterSpell(spell, projectile->GetObjectGuid());
        spellIds.clear();

        projectile->RemoveAurasDueToSpell(SPELL_LOAD_INTO_CATAPULT);
        projectile->KnockBackWithAngle(projectile->GetAngle(spell->m_targets.m_destPos.x, spell->m_targets.m_destPos.y), spell->m_targets.getSpeed() * cos(spell->m_targets.getElevation()), spell->m_targets.getSpeed() * sin(spell->m_targets.getElevation()));
        projectile->CastSpell(nullptr, SPELL_HOOKSHOT_AURA, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
    }
};

struct HookshotAura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        caster->CastSpell(nullptr, aura->GetBasePoints(), TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
        if (aura->GetAuraTicks() < 6)
            return;
        caster->RemoveAurasByCasterSpell(SPELL_HOOKSHOT_AURA, caster->GetObjectGuid());
    }
};

struct Hookshot : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (eff != EFFECT_INDEX_0)
            return true;
        Unit* caster = spell->GetCaster();
        if (!caster || !target)
            return true;
        if (caster->GetPosition().GetDistance(target->GetPosition()) > 30.f * 30.f)
            return false;
        if (target->HasAura(SPELL_SYSTEMS_SHUTDOWN))
            return false;
        caster->RemoveAurasByCasterSpell(SPELL_HOOKSHOT_AURA, caster->GetObjectGuid());
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        CreatureList turrets;
        GetCreatureListWithEntryInGrid(turrets, target, NPC_DEFENSE_TURRET, 10.f);
        for (auto& turret : turrets)
        {
            turret->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        }
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(target->GetInstanceData());
        if (!instance)
            return;
        Creature* levi = instance->GetSingleCreatureFromStorage(NPC_LEVIATHAN);
        DoBroadcastText(SAY_PLAYER_RIDE, levi);
    }
};

struct OverloadCircuit : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            return;
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        if (target->GetEntry() != NPC_LEVIATHAN)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        bool isRegularMode = target->GetMap()->IsRegularDifficulty();
        if (target->GetAuraCount(SPELL_OVERLOAD_CIRCUIT) >= 2 && isRegularMode)
        {
            target->CastSpell(nullptr, SPELL_SYSTEMS_SHUTDOWN, TRIGGERED_OLD_TRIGGERED);
        }
        else if (target->GetAuraCount(SPELL_OVERLOAD_CIRCUIT) >= 4)
        {
            target->CastSpell(nullptr, SPELL_SYSTEMS_SHUTDOWN, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct SystemsShutdown : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        if (apply)
        {
            target->SetStunned(true);
            if (target->AI())
                target->AI()->SetCombatScriptStatus(true);
            if (target->GetEntry() == NPC_LEVIATHAN_SEAT)
            {
                Unit* passenger;
                if (target && target->IsVehicle())
                    passenger = target->GetVehicleInfo()->GetPassenger(0);
                target->CastSpell(nullptr, SPELL_EJECT_PASSENGER_1, TRIGGERED_OLD_TRIGGERED);
                if (passenger)
                    target->CastSpell(passenger, SPELL_SMOKE_TRAIL, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE); // This may actually be a separate cast, not sure.
            }
        }
        else
        {
            target->SetStunned(false);
            if (target->AI())
                target->AI()->SetCombatScriptStatus(false);
        }

        if (target->GetEntry() != NPC_LEVIATHAN)
            return;
        if (apply)
        {
            CreatureList leviAdds;
            for (const uint32& entry : addEntries)
            {
                GetCreatureListWithEntryInGrid(leviAdds, target, entry, 50.f);
                for (auto add : leviAdds)
                {
                    if (add)
                    {
                        add->CastSpell(nullptr, SPELL_SYSTEMS_SHUTDOWN, TRIGGERED_OLD_TRIGGERED);
                        add->SetTarget(nullptr);
                    }
                }
            }
            target->RemoveAurasDueToSpell(SPELL_GATHERING_SPEED);
            switch (urand(0,2))
            {
                case 0: DoBroadcastText(SAY_OVERLOAD_1, target); break;
                case 1: DoBroadcastText(SAY_OVERLOAD_2, target); break;
                case 2: DoBroadcastText(SAY_OVERLOAD_3, target); break;
            }
            return;
        }
        CreatureList leviSeats;
        GetCreatureListWithEntryInGrid(leviSeats, target, NPC_LEVIATHAN_SEAT, 50.f);
        for (auto seat : leviSeats)
        {
            if (seat && seat->IsVehicle())
            {
                seat->GetVehicleInfo()->RepopulateSeat(1);
            }
        }
    }
};

struct EjectPassenger1 : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsVehicle())
            return;
        VehicleInfo* vInfo = target->GetVehicleInfo();
        if (!vInfo)
            return;
        Unit* passenger = vInfo->GetPassenger(spell->m_currentBasePoints[0]);
        if (!passenger)
            return;

        std::unordered_set<uint32> spellIds;
        for (auto aura : target->GetAurasByType(SPELL_AURA_CONTROL_VEHICLE))
        {
            if (aura->GetCasterGuid() == passenger->GetObjectGuid())
                spellIds.emplace(aura->GetId());
        }

        for (uint32 spell : spellIds)
            passenger->RemoveAurasByCasterSpell(spell, passenger->GetObjectGuid());
        spellIds.clear();
    }
};

struct SmokeTrailLeviathan : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;
        Unit* caster = spell->GetCaster();
        if (!caster || !caster->IsVehicle())
            return;
        target->KnockBackFrom(caster, 5.f, 25.f);
    }
};

struct ParachuteLeviathan : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (!caster->IsFalling())
            return;
        caster->RemoveAurasDueToSpell(aura->GetId());
        caster->CastSpell(caster, aura->GetBasePoints(), TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
    }
};

struct GrabPyrite : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target)
            return;
        if (auto transportInfo = caster->GetTransportInfo())
            if (auto vehicle = static_cast<Unit*>(transportInfo->GetTransport()))
            {
                uint32 val = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
                target->CastSpell(vehicle, val, TRIGGERED_OLD_TRIGGERED);
            }
    }
};

struct OverloadLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (target && target->IsVehicle() && caster->IsEnemy(target))
            return true;
        return false;
    }
};

struct FreyasWard : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        caster->CastSpell(caster, spell->m_currentBasePoints[1], TRIGGERED_OLD_TRIGGERED);
    }
};

struct FlamesLeviathan : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (!caster->IsEnemy(target))
            return false;
        return true;
    }
};

struct ReadyToFly : public SpellScript
{
    const std::vector<uint32> bcts = {34429, 34433};
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        if (effIdx == EFFECT_INDEX_0)
        {
            if (!target->IsBoarded())
                return;
            Unit* vehicle = dynamic_cast<Unit*>(target->GetTransportInfo()->GetTransport());
            if (!vehicle || !vehicle->IsVehicle())
                return;
            Unit* driver = vehicle->GetVehicleInfo()->GetPassenger(0);
            if (!driver)
                return;
            DoBroadcastText(spell->m_triggeredBySpellInfo->EffectBasePoints[2] + 1, spell->GetTrueCaster(), driver);
        }
        else if (effIdx == EFFECT_INDEX_1)
        {
            DoBroadcastText(bcts[urand(0,1)], target);
        }
    }
};

struct AntiAirRocket : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        if (!caster->IsEnemy(target))
            return false;
        if (target->GetEntry() == 33218)
            return false;
        return true;
    }
};

// 63605 - Rope Beam
struct RopeBeam : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster || caster->GetEntry() != 33218)
            return;
        caster->CastSpell(nullptr, SPELL_COSMETIC_PARACHITE, TRIGGERED_OLD_TRIGGERED);
        caster->GetMotionMaster()->MoveFall();
        if (caster->AI())
            caster->AI()->ResetTimer(1, 500ms);
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

    pNewScript = new Script;
    pNewScript->Name = "npc_salvaged_demolisher";
    pNewScript->GetAI = &GetNewAIInstance<npc_salvaged_demolisherAI>;
    pNewScript->pNpcSpellClick = &NpcSpellClick_npc_salvaged_demolisher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_leviathan_defense_turret";
    pNewScript->GetAI = &GetNewAIInstance<npc_leviathan_defense_turretAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_liquid_pyrite";
    pNewScript->GetAI = &GetNewAIInstance<npc_liquid_pyriteAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_pyrite_safety_container";
    pNewScript->GetAI = &GetNewAIInstance<npc_pyrite_safety_containerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_salvaged_chopper";
    pNewScript->GetAI = &GetNewAIInstance<npc_salvaged_chopperAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PursueLeviathan>("spell_pursue_leviathan");
    RegisterSpellScript<HodirsFuryLeviathan>("spell_hodirs_fury_leviathan");
    RegisterSpellScript<ThorimsHammerLeviathan>("spell_thorims_hammer_leviathan");
    RegisterSpellScript<MimironsInfernoLeviathan>("spell_mimirons_inferno_leviathan");
    RegisterSpellScript<LoadIntoCatapultLeviathan>("spell_load_into_catapult_leviathan");
    RegisterSpellScript<ThrowPassenger>("spell_throw_passenger");
    RegisterSpellScript<HookshotAura>("spell_hookshot_aura");
    RegisterSpellScript<Hookshot>("spell_hookshot");
    RegisterSpellScript<OverloadCircuit>("spell_overload_circuit");
    RegisterSpellScript<SystemsShutdown>("spell_systems_shutdown");
    RegisterSpellScript<EjectPassenger1>("spell_eject_passenger_1");
    RegisterSpellScript<SmokeTrailLeviathan>("spell_smoke_trail_leviathan");
    RegisterSpellScript<ParachuteLeviathan>("spell_parachute_leviathan");
    RegisterSpellScript<GrabPyrite>("spell_grab_crate_leviathan");
    RegisterSpellScript<OverloadLeviathan>("spell_overload_leviathan");
    RegisterSpellScript<FlamesLeviathan>("spell_flames_leviathan");
    RegisterSpellScript<FreyasWard>("spell_freyas_ward_leviathan");
    RegisterSpellScript<ReadyToFly>("spell_ready_to_fly");
    RegisterSpellScript<AntiAirRocket>("spell_anti_air_rocket");
    RegisterSpellScript<RopeBeam>("spell_rope_beam");
}
